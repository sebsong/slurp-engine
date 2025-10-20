#include "Platform.h"
#include "MacEngine.h"
#include "Settings.h"
#include "Logging.h"
#include "Debug.h"

#include <string>
#include <format>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
#include <cstring>

#if RENDER_API == OPEN_GL
#include "OpenGL.cpp"
#endif

static const char* WINDOW_TITLE = "Slurp's Up!";
static const char* SLURP_DYLIB_FILE_NAME = "libSlurpEngine.dylib";
static const char* SLURP_LOAD_DYLIB_FILE_NAME = "libSlurpEngineLoad.dylib";
static const char* RECORDING_FILE_NAME = "SlurpRecording.rec";

static bool GlobalRunning;
static MacAudioBuffer GlobalAudioBuffer;

static platform::PlatformDll GlobalPlatformDll;
static render::RenderApi GlobalRenderApi;
static memory::MemoryBlock GlobalPermanentMemory;
static memory::MemoryBlock GlobalTransientMemory;
static slurp::SlurpDll GlobalSlurpDll;
static void* GlobalSlurpLib;

// SDL gamepad tracking
static SDL_GameController* GlobalGamepads[MAX_NUM_GAMEPADS] = {nullptr};

#if DEBUG
static MacRecordingState GlobalRecordingState;
#endif

static void updateMouseButtonState(slurp::MouseState& outMouseState, slurp::MouseCode mouseCode, bool isDown) {
    slurp::DigitalInputState& inputState = outMouseState.state[mouseCode];
    inputState.transitionCount = inputState.isDown != isDown ? 1 : 0;
    inputState.isDown = isDown;
}

static void macHandleSDLEvents(
    slurp::KeyboardState& outKeyboardState,
    slurp::MouseState& outMouseState,
    const slurp::Vec2<int>& screenDimensions,
    bool& outShouldQuit
) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                outShouldQuit = true;
                GlobalRunning = false;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                SDL_Scancode scancode = event.key.keysym.scancode;
                bool isDown = (event.type == SDL_KEYDOWN);

                if (KeyboardMacCodeToSlurpCode.count(scancode) > 0) {
                    slurp::KeyboardCode code = KeyboardMacCodeToSlurpCode.at(scancode);
                    slurp::DigitalInputState& inputState = outKeyboardState.state[code];
                    inputState.transitionCount = inputState.isDown != isDown ? 1 : 0;
                    inputState.isDown = isDown;
                }
            }
            break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                uint8_t button = event.button.button;
                bool isDown = (event.type == SDL_MOUSEBUTTONDOWN);

                if (MouseMacCodeToSlurpCode.count(button) > 0) {
                    slurp::MouseCode mouseCode = MouseMacCodeToSlurpCode.at(button);
                    updateMouseButtonState(outMouseState, mouseCode, isDown);
                }
            }
            break;

            case SDL_MOUSEMOTION: {
                slurp::Vec2 mouseScreenPosition = {
                    event.motion.x,
                    screenDimensions.height - event.motion.y  // Flip Y coordinate
                };
                slurp::Mat32<float> screenToWorldMatrix = {
                    {static_cast<float>(CAMERA_WORLD_WIDTH) / screenDimensions.width, 0.f},
                    {0.f, static_cast<float>(CAMERA_WORLD_HEIGHT) / screenDimensions.height},
                    {-CAMERA_WORLD_WIDTH_MAX, -CAMERA_WORLD_HEIGHT_MAX},
                };
                outMouseState.position = mouseScreenPosition * screenToWorldMatrix;
            }
            break;

            case SDL_CONTROLLERDEVICEADDED: {
                int deviceIndex = event.cdevice.which;
                if (deviceIndex < MAX_NUM_GAMEPADS && !GlobalGamepads[deviceIndex]) {
                    GlobalGamepads[deviceIndex] = SDL_GameControllerOpen(deviceIndex);
                    if (GlobalGamepads[deviceIndex]) {
                        logging::info(std::format("Gamepad {} connected", deviceIndex));
                    }
                }
            }
            break;

            case SDL_CONTROLLERDEVICEREMOVED: {
                int instanceID = event.cdevice.which;
                for (int i = 0; i < MAX_NUM_GAMEPADS; i++) {
                    if (GlobalGamepads[i] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(GlobalGamepads[i])) == instanceID) {
                        SDL_GameControllerClose(GlobalGamepads[i]);
                        GlobalGamepads[i] = nullptr;
                        logging::info(std::format("Gamepad {} disconnected", i));
                        break;
                    }
                }
            }
            break;
        }
    }
}

static float macGetNormalizedStickValue(int16_t stickValue, int16_t deadZone) {
    if (abs(stickValue) < deadZone) { return 0.f; }
    if (stickValue > 0) { return static_cast<float>(stickValue) / 32767.f; }
    return static_cast<float>(stickValue) / 32768.f;
}

static float macGetNormalizedTriggerValue(int16_t triggerValue) {
    return static_cast<float>(triggerValue) / 32767.f;
}

static void macHandleGamepadInput(slurp::GamepadState* gamepadStates) {
    for (int i = 0; i < MAX_NUM_GAMEPADS; i++) {
        SDL_GameController* controller = GlobalGamepads[i];
        if (controller && SDL_GameControllerGetAttached(controller)) {
            slurp::GamepadState* gamepadState = &gamepadStates[i];
            gamepadState->isConnected = true;

            // Handle buttons
            for (const auto& entry : GamepadMacCodeToSlurpCode) {
                uint8_t sdlButton = entry.first;
                slurp::GamepadCode slurpCode = entry.second;
                bool isDown = SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)sdlButton) != 0;

                slurp::DigitalInputState* inputState = &gamepadState->state[slurpCode];
                inputState->transitionCount = inputState->isDown != isDown ? 1 : 0;
                inputState->isDown = isDown;
            }

            // Handle analog sticks
            int16_t leftX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
            int16_t leftY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
            slurp::AnalogStickInputState& leftStickState = gamepadState->leftStick;
            leftStickState.start = leftStickState.end;
            leftStickState.end.x = macGetNormalizedStickValue(leftX, 8000);
            leftStickState.end.y = macGetNormalizedStickValue(leftY, 8000);

            int16_t rightX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
            int16_t rightY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
            slurp::AnalogStickInputState& rightStickState = gamepadState->rightStick;
            rightStickState.start = rightStickState.end;
            rightStickState.end.x = macGetNormalizedStickValue(rightX, 8000);
            rightStickState.end.y = macGetNormalizedStickValue(rightY, 8000);

            // Handle triggers
            int16_t leftTrigger = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            slurp::AnalogTriggerInputState& leftTriggerState = gamepadState->leftTrigger;
            leftTriggerState.start = leftTriggerState.end;
            leftTriggerState.end = macGetNormalizedTriggerValue(leftTrigger);

            int16_t rightTrigger = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            slurp::AnalogTriggerInputState& rightTriggerState = gamepadState->rightTrigger;
            rightTriggerState.start = rightTriggerState.end;
            rightTriggerState.end = macGetNormalizedTriggerValue(rightTrigger);
        } else {
            gamepadStates[i].isConnected = false;
        }
    }
}

// Audio callback for SDL
static void macAudioCallback(void* userdata, uint8_t* stream, int len) {
    audio::AudioBuffer buffer = {};
    buffer.samples = reinterpret_cast<audio::StereoAudioSample*>(stream);
    buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    buffer.numSamplesToWrite = len / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.bufferAudio(buffer);
}

static void macInitSDLAudio() {
    SDL_AudioSpec desired = {};
    desired.freq = AUDIO_SAMPLES_PER_SECOND;
    desired.format = AUDIO_S16LSB;
    desired.channels = NUM_AUDIO_CHANNELS;
    desired.samples = 1024;
    desired.callback = macAudioCallback;

    SDL_AudioSpec obtained;
    GlobalAudioBuffer.device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);

    if (GlobalAudioBuffer.device == 0) {
        logging::error(std::format("Failed to open audio device: {}", SDL_GetError()));
        return;
    }

    GlobalAudioBuffer.samplesPerSec = obtained.freq;
    GlobalAudioBuffer.bytesPerSample = TOTAL_AUDIO_SAMPLE_SIZE;
    GlobalAudioBuffer.bufferSizeBytes = AUDIO_BUFFER_SECONDS * obtained.freq * GlobalAudioBuffer.bytesPerSample;

    SDL_PauseAudioDevice(GlobalAudioBuffer.device, 0);  // Start playing
    logging::info("SDL Audio initialized");
}

static uint64_t macGetPerformanceFrequency() {
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    return (1000000000ULL * info.denom) / info.numer;
}

static float macGetFrameMillis(
    MacTimingInfo startTimingInfo,
    uint64_t& outPerformanceCounterEnd
) {
    outPerformanceCounterEnd = mach_absolute_time();
    return (outPerformanceCounterEnd - startTimingInfo.performanceCounter) * 1000.f / startTimingInfo.performanceCounterFrequency;
}

static void macStallFrameToTarget(
    float targetMillisPerFrame,
    MacTimingInfo startTimingInfo
) {
    uint64_t performanceCounterEnd;
    float frameMillis = macGetFrameMillis(startTimingInfo, performanceCounterEnd);
    if (frameMillis >= targetMillisPerFrame) {
        return;
    }

    // Use SDL_Delay for coarse sleeping
    float remainingMs = targetMillisPerFrame - frameMillis;
    if (remainingMs > 1.0f) {
        SDL_Delay(static_cast<uint32_t>(remainingMs - 1.0f));
    }

    // Spin wait for precision
    while (frameMillis < targetMillisPerFrame) {
        frameMillis = macGetFrameMillis(startTimingInfo, performanceCounterEnd);
    }
}

static void macCaptureAndLogPerformance(
    uint64_t& startProcessorCycle,
    MacTimingInfo& startTimingInfo
) {
    uint64_t processorCycleEnd = __builtin_readcyclecounter();
    uint64_t performanceCounterEnd;

    float frameMillis = macGetFrameMillis(startTimingInfo, performanceCounterEnd);
    int fps = static_cast<int>(1000 / frameMillis);
    int frameProcessorMCycles = static_cast<int>((processorCycleEnd - startProcessorCycle) / 1000 / 1000);

#if VERBOSE_LOGGING
    logging::info(std::format("Frame: {:.2f}ms {}fps {} processor mega-cycles", frameMillis, fps, frameProcessorMCycles));
#endif

    startProcessorCycle = processorCycleEnd;
    startTimingInfo.performanceCounter = performanceCounterEnd;
}

template<typename T>
static void macLoadLibFn(T*& out, const char* fnName, T* stubFn, void* lib) {
    out = reinterpret_cast<T*>(dlsym(lib, fnName));
    if (!out) {
        logging::error(std::format("Failed to load lib function: {}", fnName));
        ASSERT(out);
        out = stubFn;
    }
}

static std::string getExecutableDir() {
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        char* lastSlash = strrchr(buffer, '/');
        if (lastSlash) {
            *lastSlash = '\0';
        }
        return std::string(buffer);
    }
    return ".";
}

static std::string getLocalFilePath(const char* filename) {
    return getExecutableDir() + "/" + filename;
}

static void macLoadSlurpLib(const char* dylibFilePath, const char* dylibLoadFilePath) {
    // Copy dylib to load path
    std::string copyCmd = std::format("cp \"{}\" \"{}\"", dylibFilePath, dylibLoadFilePath);
    system(copyCmd.c_str());

    GlobalSlurpLib = dlopen(dylibLoadFilePath, RTLD_NOW);
    if (!GlobalSlurpLib) {
        logging::error(std::format("Failed to load libSlurpEngine.dylib: {}", dlerror()));
    } else {
        macLoadLibFn<slurp::dyn_init>(
            GlobalSlurpDll.init,
            "init",
            slurp::stub_init,
            GlobalSlurpLib
        );
        macLoadLibFn<slurp::dyn_frameStart>(
            GlobalSlurpDll.frameStart,
            "frameStart",
            slurp::stub_frameStart,
            GlobalSlurpLib
        );
        macLoadLibFn<slurp::dyn_handleInput>(
            GlobalSlurpDll.handleInput,
            "handleInput",
            slurp::stub_handleInput,
            GlobalSlurpLib
        );
        macLoadLibFn<slurp::dyn_bufferAudio>(
            GlobalSlurpDll.bufferAudio,
            "bufferAudio",
            slurp::stub_bufferAudio,
            GlobalSlurpLib
        );
        macLoadLibFn<slurp::dyn_updateAndRender>(
            GlobalSlurpDll.updateAndRender,
            "updateAndRender",
            slurp::stub_updateAndRender,
            GlobalSlurpLib
        );
        macLoadLibFn<slurp::dyn_frameEnd>(
            GlobalSlurpDll.frameEnd,
            "frameEnd",
            slurp::stub_frameEnd,
            GlobalSlurpLib
        );
    }
}

static void macUnloadSlurpLib() {
    if (GlobalSlurpLib) {
        dlclose(GlobalSlurpLib);
    }
    GlobalSlurpDll = slurp::SlurpDll();
}

static void macTryReloadSlurpLib(const char* dylibFilePath, const char* dylibLoadFilePath) {
    struct stat fileStat;
    static time_t previousWriteTime = 0;

    if (stat(dylibFilePath, &fileStat) == 0) {
        if (fileStat.st_mtime != previousWriteTime) {
            previousWriteTime = fileStat.st_mtime;
            macUnloadSlurpLib();
            macLoadSlurpLib(dylibFilePath, dylibLoadFilePath);
            GlobalSlurpDll.init(GlobalPermanentMemory, GlobalTransientMemory, GlobalPlatformDll, GlobalRenderApi);
        }
    }
}

static void macAllocateGameMemory(memory::MemoryBlock& outPermanentMemory, memory::MemoryBlock& outTransientMemory) {
    size_t permanentMemorySizeBytes = PERMANENT_ARENA_SIZE;
    size_t transientMemorySizeBytes = TRANSIENT_ARENA_SIZE;

    void* memory = mmap(
        nullptr,
        permanentMemorySizeBytes + transientMemorySizeBytes,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (memory == MAP_FAILED) {
        logging::error("Failed to allocate game memory");
        return;
    }

    outPermanentMemory = memory::MemoryBlock(
        static_cast<types::byte*>(memory),
        permanentMemorySizeBytes
    );
    outTransientMemory = memory::MemoryBlock(
        static_cast<types::byte*>(memory) + permanentMemorySizeBytes,
        transientMemorySizeBytes
    );
}

#if DEBUG
PLATFORM_DEBUG_READ_FILE(platform::DEBUG_readFile) {
    platform::DEBUG_FileReadResult result = {};
    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        logging::error("Invalid file handle");
        return result;
    }

    struct stat fileStat;
    fstat(fd, &fileStat);
    size_t fileSize = fileStat.st_size;

    void* buffer = mmap(nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ssize_t bytesRead = read(fd, buffer, fileSize);
    close(fd);

    if (bytesRead != static_cast<ssize_t>(fileSize)) {
        logging::error("Could not read file");
        munmap(buffer, fileSize);
        return result;
    }

    result.fileContents = buffer;
    result.sizeBytes = static_cast<uint32_t>(fileSize);
    return result;
}

PLATFORM_DEBUG_WRITE_FILE(platform::DEBUG_writeFile) {
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        logging::error("Invalid file handle");
        return false;
    }

    ssize_t bytesWritten = write(fd, fileContents, sizeBytes);
    close(fd);

    if (bytesWritten != static_cast<ssize_t>(sizeBytes)) {
        logging::error("Could not write file");
        return false;
    }
    return true;
}

PLATFORM_DEBUG_FREE_MEMORY(platform::DEBUG_freeMemory) {
    if (memory) {
        // Note: We'd need to track the size separately for proper munmap
        // For now, we'll just not implement this fully
    }
}

PLATFORM_DEBUG_TOGGLE_PAUSE(platform::DEBUG_togglePause) {
    GlobalRecordingState.isPaused = !GlobalRecordingState.isPaused;
}

PLATFORM_DEBUG_BEGIN_RECORDING(platform::DEBUG_beginRecording) {
    GlobalRecordingState.recordingFile = SDL_RWFromFile(getLocalFilePath(RECORDING_FILE_NAME).c_str(), "wb");
    SDL_RWwrite(GlobalRecordingState.recordingFile, GlobalPermanentMemory.memory, GlobalPermanentMemory.size, 1);
    GlobalRecordingState.isRecording = true;
}

template<typename T>
static void macRecordStateMap(std::unordered_map<T, slurp::DigitalInputState> stateMap) {
    size_t numStates = stateMap.size();
    SDL_RWwrite(GlobalRecordingState.recordingFile, &numStates, sizeof(numStates), 1);
    for (const std::pair<const T, slurp::DigitalInputState>& entry: stateMap) {
        SDL_RWwrite(GlobalRecordingState.recordingFile, &entry, sizeof(entry), 1);
    }
}

static void macRecordInput(
    const slurp::MouseState& mouseState,
    const slurp::KeyboardState& keyboardState,
    slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS]
) {
    SDL_RWwrite(GlobalRecordingState.recordingFile, &mouseState.position, sizeof(mouseState.position), 1);
    macRecordStateMap(mouseState.state);
    macRecordStateMap(keyboardState.state);

    for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
        slurp::GamepadState& gamepadState = gamepadStates[gamepadIndex];
        SDL_RWwrite(GlobalRecordingState.recordingFile, &gamepadState.isConnected, sizeof(gamepadState.isConnected), 1);
        SDL_RWwrite(GlobalRecordingState.recordingFile, &gamepadState.leftStick, sizeof(gamepadState.leftStick), 1);
        SDL_RWwrite(GlobalRecordingState.recordingFile, &gamepadState.rightStick, sizeof(gamepadState.rightStick), 1);
        SDL_RWwrite(GlobalRecordingState.recordingFile, &gamepadState.leftTrigger, sizeof(gamepadState.leftTrigger), 1);
        SDL_RWwrite(GlobalRecordingState.recordingFile, &gamepadState.rightTrigger, sizeof(gamepadState.rightTrigger), 1);
        macRecordStateMap(gamepadState.state);
    }
}

PLATFORM_DEBUG_END_RECORDING(platform::DEBUG_endRecording) {
    GlobalRecordingState.isRecording = false;
    SDL_RWclose(GlobalRecordingState.recordingFile);
}

PLATFORM_DEBUG_BEGIN_PLAYBACK(platform::DEBUG_beginPlayback) {
    GlobalRecordingState.recordingFile = SDL_RWFromFile(getLocalFilePath(RECORDING_FILE_NAME).c_str(), "rb");
    SDL_RWread(GlobalRecordingState.recordingFile, GlobalPermanentMemory.memory, GlobalPermanentMemory.size, 1);
    GlobalRecordingState.isPlayingBack = true;
    GlobalRecordingState.onPlaybackEnd = onPlaybackEnd;
}

template<typename T>
static void macReadInputStateMap(std::unordered_map<T, slurp::DigitalInputState>& outStateMap) {
    size_t numStates = 0;
    SDL_RWread(GlobalRecordingState.recordingFile, &numStates, sizeof(numStates), 1);
    outStateMap.clear();
    for (size_t i = 0; i < numStates; i++) {
        std::pair<const T, slurp::DigitalInputState> entry;
        SDL_RWread(GlobalRecordingState.recordingFile, &entry, sizeof(entry), 1);
        outStateMap.insert(entry);
    }
}

static void macReadInputRecording(
    slurp::MouseState& outMouseState,
    slurp::KeyboardState& outKeyboardState,
    slurp::GamepadState outGamepadStates[MAX_NUM_GAMEPADS]
) {
    size_t bytesRead = SDL_RWread(GlobalRecordingState.recordingFile, &outMouseState.position, sizeof(outMouseState.position), 1);
    if (bytesRead == 0) {
        GlobalRecordingState.isPlayingBack = false;
        SDL_RWclose(GlobalRecordingState.recordingFile);
        GlobalRecordingState.onPlaybackEnd();
        return;
    }

    macReadInputStateMap(outMouseState.state);
    macReadInputStateMap(outKeyboardState.state);

    for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
        slurp::GamepadState& outGamepadState = outGamepadStates[gamepadIndex];
        SDL_RWread(GlobalRecordingState.recordingFile, &outGamepadState.isConnected, sizeof(outGamepadState.isConnected), 1);
        SDL_RWread(GlobalRecordingState.recordingFile, &outGamepadState.leftStick, sizeof(outGamepadState.leftStick), 1);
        SDL_RWread(GlobalRecordingState.recordingFile, &outGamepadState.rightStick, sizeof(outGamepadState.rightStick), 1);
        SDL_RWread(GlobalRecordingState.recordingFile, &outGamepadState.leftTrigger, sizeof(outGamepadState.leftTrigger), 1);
        SDL_RWread(GlobalRecordingState.recordingFile, &outGamepadState.rightTrigger, sizeof(outGamepadState.rightTrigger), 1);
        macReadInputStateMap(outGamepadState.state);
    }
}
#endif

PLATFORM_VIBRATE_GAMEPAD(platform::vibrateGamepad) {
    if (gamepadIndex >= 0 && gamepadIndex < MAX_NUM_GAMEPADS && GlobalGamepads[gamepadIndex]) {
        uint16_t lowFreq = static_cast<uint16_t>(leftMotorSpeed * 0xFFFF);
        uint16_t highFreq = static_cast<uint16_t>(rightMotorSpeed * 0xFFFF);
        SDL_GameControllerRumble(GlobalGamepads[gamepadIndex], lowFreq, highFreq, 100);
    }
}

PLATFORM_SHUTDOWN(platform::shutdown) {
    GlobalRunning = false;
}

static platform::PlatformDll loadPlatformDll() {
    platform::PlatformDll platformDll = {};
    platformDll.vibrateGamepad = platform::vibrateGamepad;
    platformDll.shutdown = platform::shutdown;
#if DEBUG
    platformDll.DEBUG_readFile = platform::DEBUG_readFile;
    platformDll.DEBUG_writeFile = platform::DEBUG_writeFile;
    platformDll.DEBUG_freeMemory = platform::DEBUG_freeMemory;
    platformDll.DEBUG_togglePause = platform::DEBUG_togglePause;
    platformDll.DEBUG_beginRecording = platform::DEBUG_beginRecording;
    platformDll.DEBUG_endRecording = platform::DEBUG_endRecording;
    platformDll.DEBUG_beginPlayback = platform::DEBUG_beginPlayback;
#endif
    return platformDll;
}

static render::RenderApi loadRenderApi() {
    render::RenderApi renderApi = {};
    renderApi.setBackgroundColor = opengl::setBackgroundColor;
    renderApi.createTexture = opengl::createTexture;
    renderApi.createShaderProgram = opengl::createShaderProgram;
    renderApi.genVertexArrayBuffer = opengl::genVertexArrayBuffer;
    renderApi.genElementArrayBuffer = opengl::genElementArrayBuffer;
    renderApi.drawVertexArray = opengl::drawVertexArray;
    renderApi.drawElementArray = opengl::drawElementArray;
    renderApi.drawLine = opengl::drawLine;
    renderApi.deleteResources = opengl::deleteResources;
    return renderApi;
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        logging::error(std::format("Failed to initialize SDL: {}", SDL_GetError()));
        return 1;
    }

#if RENDER_API == OPEN_GL
    opengl::OpenGLRenderWindow renderWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, WINDOW_TITLE);
    if (!renderWindow.isValid()) {
        SDL_Quit();
        return 1;
    }
#endif

    GlobalRunning = true;

    std::string dylibFilePathStr = getLocalFilePath(SLURP_DYLIB_FILE_NAME);
    const char* dylibFilePath = dylibFilePathStr.c_str();
    std::string dylibLoadFilePathStr = getLocalFilePath(SLURP_LOAD_DYLIB_FILE_NAME);
    const char* dylibLoadFilePath = dylibLoadFilePathStr.c_str();
    macLoadSlurpLib(dylibFilePath, dylibLoadFilePath);

    GlobalPlatformDll = loadPlatformDll();
    GlobalRenderApi = loadRenderApi();
    macAllocateGameMemory(GlobalPermanentMemory, GlobalTransientMemory);
    macTryReloadSlurpLib(dylibFilePath, dylibLoadFilePath);

#if DEBUG
    uint32_t targetFramesPerSecond = DEBUG_MONITOR_REFRESH_RATE;
#else
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    uint32_t targetFramesPerSecond = displayMode.refresh_rate > 0 ? displayMode.refresh_rate : DEFAULT_MONITOR_REFRESH_RATE;
#endif
    float targetSecondsPerFrame = 1.f / targetFramesPerSecond;
    float targetMillisPerFrame = targetSecondsPerFrame * 1000.f;

    slurp::MouseState mouseState;
    slurp::KeyboardState keyboardState;
    slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS];

#if !DEBUG_SHOW_MOUSE_CURSOR
    SDL_ShowCursor(SDL_DISABLE);
#endif

    macInitSDLAudio();

    uint64_t startProcessorCycle = __builtin_readcyclecounter();
    uint64_t performanceFrequency = macGetPerformanceFrequency();
    MacTimingInfo startTimingInfo = {
        mach_absolute_time(),
        performanceFrequency
    };

    bool shouldQuit = false;
    while (GlobalRunning && !shouldQuit) {
        macTryReloadSlurpLib(dylibFilePath, dylibLoadFilePath);

        GlobalSlurpDll.frameStart();

        // Clear transition counts
        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry: keyboardState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        for (std::pair<const slurp::MouseCode, slurp::DigitalInputState>& entry: mouseState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }

        macHandleSDLEvents(keyboardState, mouseState, renderWindow.getDimensions(), shouldQuit);
        macHandleGamepadInput(gamepadStates);

#if DEBUG
        if (GlobalRecordingState.isRecording) { macRecordInput(mouseState, keyboardState, gamepadStates); }
        if (GlobalRecordingState.isPlayingBack) { macReadInputRecording(mouseState, keyboardState, gamepadStates); }
#endif

        GlobalSlurpDll.handleInput(mouseState, keyboardState, gamepadStates);

#if DEBUG
        if (GlobalRecordingState.isPaused) { continue; }
#endif

        GlobalSlurpDll.updateAndRender(targetSecondsPerFrame);

        macStallFrameToTarget(targetMillisPerFrame, startTimingInfo);
        macCaptureAndLogPerformance(startProcessorCycle, startTimingInfo);

        renderWindow.flip();
        if (renderWindow.shouldTerminate()) {
            GlobalRunning = false;
        }

        GlobalSlurpDll.frameEnd();
    }

    // Cleanup
    for (int i = 0; i < MAX_NUM_GAMEPADS; i++) {
        if (GlobalGamepads[i]) {
            SDL_GameControllerClose(GlobalGamepads[i]);
        }
    }

    if (GlobalAudioBuffer.device) {
        SDL_CloseAudioDevice(GlobalAudioBuffer.device);
    }

    SDL_Quit();
    return 0;
}
