#include "Engine.h"

#include "Logging.h"
#include "Platform.h"
#include "RenderApi.h"
#include "Settings.h"
#include "SlurpEngine.h"

#include <SDL3/SDL.h>
#include <filesystem>

#if PLATFORM_WINDOWS
#include "Win32.cpp"
#elif PLATFORM_MAC
#include "MacOS.cpp"
#endif

#if RENDER_API == OPEN_GL
#include "OpenGL.cpp"
#endif

static bool GlobalRunning;

PLATFORM_SHUTDOWN(platform::shutdown) {
    GlobalRunning = false;
}

static std::string getLocalFilePath(const char* filename) {
    return std::filesystem::path(SDL_GetBasePath()).replace_filename(filename).string();
}

static bool initSDL(SDL_Window*& outWindow, SDL_AudioStream*& outAudioStream) {
    if (!SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_IDENTIFIER)) {
        logging::error("Failed to set SDL app metadata.");
        return false;
    }
#if PLATFORM_WINDOWS
    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "directsound");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        logging::error("Failed to initialize SDL.");
        return false;
    }
    SDL_WindowFlags windowFLags{};
#if RENDER_API == OPEN_GL
    windowFLags |= SDL_WINDOW_OPENGL;
#endif

#if FULLSCREEN
    windowFLags |= SDL_WINDOW_FULLSCREEN;
#else
    windowFLags |= SDL_WINDOW_RESIZABLE;
#endif
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, DISPLAY_WIDTH, DISPLAY_HEIGHT, windowFLags);
    outWindow = window;

#if HIDE_CURSOR
    SDL_HideCursor();
#endif

#if RENDER_API == OPEN_GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        logging::error("Failed to create OpenGL context.");
        return false;
    }
    if (!SDL_GL_MakeCurrent(window, glContext)) {
        logging::error("Failed to make OpenGL context current.");
        return false;
    }
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        logging::error("Failed to load glad GL loader");
        return false;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif

    /* initialize audio */
    SDL_AudioSpec audioSpec{
        SDL_AUDIO_S16LE,
        NUM_AUDIO_CHANNELS,
        AUDIO_SAMPLES_PER_SECOND
    };
    SDL_AudioDeviceID audioDeviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    SDL_AudioStream* audioStream = SDL_CreateAudioStream(nullptr, &audioSpec);
    outAudioStream = audioStream;
    if (!SDL_BindAudioStream(audioDeviceId, audioStream)) {
        ASSERT_LOG(false, "Failed to bind audio stream.");
    }

    return true;
}

static void allocateMemoryArenas(memory::MemoryArena& outPermanentMemory, memory::MemoryArena& outTransientMemory) {
    size_t permanentMemorySizeBytes = PERMANENT_ARENA_SIZE;
    size_t transientMemorySizeBytes = TRANSIENT_ARENA_SIZE;
    size_t size = permanentMemorySizeBytes + transientMemorySizeBytes;
    types::byte* memory = platform::allocateMemory(size);
    memory::MemoryArena fullMemory("Full Memory", {memory, size});
    outPermanentMemory = fullMemory.allocateSubArena("Permanent Memory", permanentMemorySizeBytes);
    outTransientMemory = fullMemory.allocateSubArena("Transient Memory", transientMemorySizeBytes);
}

static platform::PlatformDll loadPlatformLib() {
    platform::PlatformDll platformLib = {};
    // platformDll.vibrateGamepad = platform::vibrateGamepad;
    platformLib.shutdown = platform::shutdown;
    // #if DEBUG
    //     platformDll.DEBUG_readFile = platform::DEBUG_readFile;
    //     platformDll.DEBUG_writeFile = platform::DEBUG_writeFile;
    //     platformDll.DEBUG_freeMemory = platform::DEBUG_freeMemory;
    //     platformDll.DEBUG_togglePause = platform::DEBUG_togglePause;
    //     platformDll.DEBUG_beginRecording = platform::DEBUG_beginRecording;
    //     platformDll.DEBUG_endRecording = platform::DEBUG_endRecording;
    //     platformDll.DEBUG_beginPlayback = platform::DEBUG_beginPlayback;
    // #endif
    return platformLib;
}

static render::RenderApi loadRenderApi() {
    render::RenderApi renderApi = {};
    renderApi.setBackgroundColor = open_gl::setBackgroundColor;
    renderApi.createTexture = open_gl::createTexture;
    renderApi.createShaderProgram = open_gl::createShaderProgram;
    renderApi.bindShaderUniformFloat = open_gl::bindShaderUniformFloat;
    renderApi.bindShaderUniformBool = open_gl::bindShaderUniformBool;
    renderApi.genVertexArrayBuffer = open_gl::genVertexArrayBuffer;
    renderApi.genElementArrayBuffer = open_gl::genElementArrayBuffer;
    renderApi.drawVertexArray = open_gl::drawVertexArray;
    renderApi.drawElementArray = open_gl::drawElementArray;
    renderApi.drawPoint = open_gl::drawPoint;
    renderApi.drawLine = open_gl::drawLine;
    renderApi.deleteResources = open_gl::deleteResources;
    return renderApi;
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_AudioStream* audioStream;
    memory::MemoryArena permanentMemory;
    memory::MemoryArena transientMemory;
    platform::PlatformDll platformLib;
    render::RenderApi renderApi;
    slurp::SlurpDll slurpLib;
    slurp::MouseState mouseState{};
    slurp::KeyboardState keyboardState{};
    std::unordered_map<SDL_JoystickID, uint8_t> sdlJoystickIdToGamepadIdx;
    slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS]{};

    if (!initSDL(window, audioStream)) {
        logging::error("Failed to initialize SDL.");
        return 1;
    }

    /* load libraries and apis */
    platformLib = loadPlatformLib();
    renderApi = loadRenderApi();
    std::string libFilePathStr = getLocalFilePath(SLURP_LIB_FILE_NAME);
    const char* libFilePath = libFilePathStr.c_str();
    std::string libLoadFilePathStr = getLocalFilePath(SLURP_LIB_LOAD_FILE_NAME);
    const char* libLoadFilePath = libLoadFilePathStr.c_str();
    slurpLib = platform::loadSlurpLib(libFilePath);
    allocateMemoryArenas(permanentMemory, transientMemory);
    slurpLib.init(permanentMemory, transientMemory, platformLib, renderApi, false);

#if DEBUG
    uint32_t targetFramesPerSecond = DEBUG_MONITOR_REFRESH_RATE;
#else
    uint32_t targetFramesPerSecond = DEFAULT_MONITOR_REFRESH_RATE;
#endif
    float targetSecondsPerFrame = 1.f / targetFramesPerSecond;
    uint64_t targetNanosPerFrame = static_cast<uint64_t>(static_cast<double>(targetSecondsPerFrame) * 1'000'000'000.0);

    GlobalRunning = true;
    while (GlobalRunning) {
        uint64_t frameStartNanos = SDL_GetTicksNS();
        slurpLib.frameStart();

        /* reset input state */
        for (std::pair<const slurp::MouseCode, slurp::DigitalInputState>& entry: mouseState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry: keyboardState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        /* handle SDL events */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_MOUSE_MOTION: {
                    int windowWidth;
                    int windowHeight;
                    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

                    SDL_MouseMotionEvent mouseMotionEvent = event.motion;
                    slurp::Vec2 mouseScreenPosition = {mouseMotionEvent.x, mouseMotionEvent.y};
                    // TODO: maybe this should be cached and updated whenever the screen dimensions (infrequently) update
                    slurp::Mat32<float> screenToWorldMatrix = {
                        {static_cast<float>(WORLD_WIDTH) / windowWidth, 0.f},
                        {0.f, -static_cast<float>(WORLD_HEIGHT) / windowHeight},
                        {-WORLD_WIDTH_MAX, WORLD_HEIGHT_MAX},
                    };
                    mouseState.position = mouseScreenPosition * screenToWorldMatrix;
                }
                break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    SDL_MouseButtonEvent mouseButtonEvent = event.button;
                    SDL_MouseButtonCode sdlCode = mouseButtonEvent.button;
                    if (MouseButtonSDLCodeToSlurpCode.contains(sdlCode)) {
                        slurp::MouseCode slurpCode = MouseButtonSDLCodeToSlurpCode.at(sdlCode);
                        slurp::DigitalInputState& inputState = mouseState.state[slurpCode];
                        inputState.transitionCount = mouseButtonEvent.down ? mouseButtonEvent.clicks : 1;
                        inputState.isDown = mouseButtonEvent.down;
                    } else {
                        logging::error(std::format("Mouse button code not registered: {}", SDL_GetKeyName(sdlCode)));
                    }
                }
                break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    SDL_KeyboardEvent keyboardEvent = event.key;
                    SDL_Keycode sdlCode = keyboardEvent.key;

                    if (KeyboardSDLCodeToSlurpCode.contains(sdlCode)) {
                        slurp::KeyboardCode slurpCode = KeyboardSDLCodeToSlurpCode.at(sdlCode);
                        slurp::DigitalInputState& inputState = keyboardState.state[slurpCode];
                        // TODO: update transition count computation if we poll multiple times per frame
                        inputState.transitionCount = keyboardEvent.repeat ? 0 : 1;
                        inputState.isDown = keyboardEvent.down;
                    } else {
                        logging::error(std::format("Keyboard code not registered: {}", SDL_GetKeyName(sdlCode)));
                    }
                }
                break;
                case SDL_EVENT_GAMEPAD_ADDED: {
                    SDL_GamepadDeviceEvent gamepadDeviceEvent = event.gdevice;
                    if (sdlJoystickIdToGamepadIdx.size() < MAX_NUM_GAMEPADS &&
                        !sdlJoystickIdToGamepadIdx.contains(gamepadDeviceEvent.which)
                    ) {
                        sdlJoystickIdToGamepadIdx[gamepadDeviceEvent.which] = static_cast<uint8_t>(
                            sdlJoystickIdToGamepadIdx.size()
                        );
                    }
                }
                break;
                case SDL_EVENT_GAMEPAD_REMOVED: {
                    SDL_GamepadDeviceEvent gamepadDeviceEvent = event.gdevice;
                    sdlJoystickIdToGamepadIdx.erase(gamepadDeviceEvent.which);
                }
                break;
                case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                    SDL_GamepadAxisEvent gamepadAxisEvent = event.gaxis;
                    uint8_t gamepadIndex = sdlJoystickIdToGamepadIdx[gamepadAxisEvent.which];
                    float* axisStateStart;
                    float* axisStateEnd;
                    switch (gamepadAxisEvent.axis) {
                        case SDL_GAMEPAD_AXIS_LEFTX: {
                            slurp::AnalogStickInputState& inputState = gamepadStates[gamepadIndex].leftStick;
                            axisStateStart = &inputState.start.x;
                            axisStateEnd = &inputState.end.x;
                        }
                        break;
                        case SDL_GAMEPAD_AXIS_LEFTY: {
                            slurp::AnalogStickInputState& inputState = gamepadStates[gamepadIndex].leftStick;
                            axisStateStart = &inputState.start.y;
                            axisStateEnd = &inputState.end.y;
                        }
                        break;
                        case SDL_GAMEPAD_AXIS_RIGHTX: {
                            slurp::AnalogStickInputState& inputState = gamepadStates[gamepadIndex].rightStick;
                            axisStateStart = &inputState.start.x;
                            axisStateEnd = &inputState.end.x;
                        }
                        break;
                        case SDL_GAMEPAD_AXIS_RIGHTY: {
                            slurp::AnalogStickInputState& inputState = gamepadStates[gamepadIndex].rightStick;
                            axisStateStart = &inputState.start.y;
                            axisStateEnd = &inputState.end.y;
                        }
                        break;
                        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER: {
                            slurp::AnalogTriggerInputState& inputState = gamepadStates[gamepadIndex].leftTrigger;
                            axisStateStart = &inputState.start;
                            axisStateEnd = &inputState.end;
                        }
                        break;
                        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER: {
                            slurp::AnalogTriggerInputState& inputState = gamepadStates[gamepadIndex].rightTrigger;
                            axisStateStart = &inputState.start;
                            axisStateEnd = &inputState.end;
                        }
                        break;
                        default: {
                            logging::error(std::format("Gamepad axis not registered: {}", gamepadAxisEvent.axis));
                            continue;
                        }
                    }
                    *axisStateStart = *axisStateEnd;
                    *axisStateEnd = gamepadAxisEvent.value;
                }
                break;
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                    SDL_GamepadButtonEvent gamepadButtonEvent = event.gbutton;
                    SDL_GamepadButtonCode sdlCode = gamepadButtonEvent.button;

                    if (GamepadButtonSDLCodeToSlurpCode.contains(sdlCode) &&
                        sdlJoystickIdToGamepadIdx.contains(gamepadButtonEvent.which)
                    ) {
                        slurp::GamepadCode slurpCode = GamepadButtonSDLCodeToSlurpCode.at(sdlCode);
                        uint8_t gamepadIndex = sdlJoystickIdToGamepadIdx[gamepadButtonEvent.which];
                        slurp::DigitalInputState& inputState = gamepadStates[gamepadIndex].state[slurpCode];
                        // TODO: update transition count computation if we poll multiple times per frame
                        inputState.transitionCount = gamepadButtonEvent.down != inputState.isDown;
                        inputState.isDown = gamepadButtonEvent.down;
                    } else {
                        logging::error(std::format("Gamepad code not registered: {}", SDL_GetKeyName(sdlCode)));
                    }
                }
                break;
                case SDL_EVENT_WINDOW_RESIZED: {
#if RENDER_API == OPEN_GL
                    glViewport(0, 0, event.window.data1, event.window.data2);
#endif
                }
                break;
                case SDL_EVENT_QUIT: {
                    GlobalRunning = false;
                }
                break;
                default: {}
            }
        }
        slurpLib.handleInput(mouseState, keyboardState, gamepadStates);

        /* update and render */
#if RENDER_API == OPEN_GL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
        slurpLib.updateAndRender(targetSecondsPerFrame);
#if RENDER_API == OPEN_GL
        SDL_GL_SwapWindow(window);
#endif

        /* audio */
        // TODO: clean up audio types and api
        constexpr int targetNumAudioSamplesToBuffer = static_cast<int>(
            AUDIO_SAMPLES_PER_SECOND * AUDIO_BUFFER_WRITE_AHEAD_SECONDS);
        audio::StereoAudioSample audioSampleBuffer[targetNumAudioSamplesToBuffer];
        int numAudioSamplesBuffered = SDL_GetAudioStreamQueued(audioStream) / sizeof(audio::StereoAudioSample);
        int numAudioSamplesToBuffer = std::max(targetNumAudioSamplesToBuffer - numAudioSamplesBuffered, 0);
        audio::AudioBuffer audioBuffer{
            audioSampleBuffer,
            AUDIO_SAMPLES_PER_SECOND,
            numAudioSamplesToBuffer
        };
        slurpLib.bufferAudio(audioBuffer);
        SDL_PutAudioStreamData(
            audioStream,
            audioSampleBuffer,
            numAudioSamplesToBuffer * sizeof(audio::StereoAudioSample)
        );

        slurpLib.frameEnd();
        uint64_t frameNanos = SDL_GetTicksNS() - frameStartNanos;
        if (frameNanos < targetNanosPerFrame) {
            SDL_DelayPrecise(targetNanosPerFrame - frameNanos);
        }
    }
    return 0;
}
