#include "Platform.h"
#include "WinEngine.h"
#include "Settings.h"
#include "Logging.h"
#include "Debug.h"

#include <string>
#include <format>
#include <shlwapi.h>
#include <windowsx.h>
#include <mmreg.h>


#if RENDER_API == OPEN_GL
#include "OpenGL.cpp"
#endif

static const char* WINDOW_TITLE = "Slurp's Up!";
static const char* SLURP_DLL_FILE_NAME = "SlurpEngine.dll";
static const char* SLURP_DLL_MARKER_FILE_NAME = "SlurpEngine.dll.marker";
static const char* SLURP_LOAD_DLL_FILE_NAME = "SlurpEngineLoad.dll";
static const char* RECORDING_FILE_NAME = "SlurpRecording.rec";

static bool GlobalRunning;

static WinAudioBuffer GlobalAudioBuffer;

static platform::PlatformDll GlobalPlatformDll;
static render::RenderApi GlobalRenderApi;
static memory::MemoryArena GlobalPermanentMemory;
static memory::MemoryArena GlobalTransientMemory;
static slurp::SlurpDll GlobalSlurpDll;
static HMODULE GlobalSlurpLib;

#if DEBUG
static WinRecordingState GlobalRecordingState;
#endif

static void updateMouseButtonState(slurp::MouseState& outMouseState, slurp::MouseCode mouseCode, bool isDown) {
    slurp::DigitalInputState& inputState = outMouseState.state[mouseCode];
    inputState.transitionCount = inputState.isDown != isDown ? 1 : 0;;
    inputState.isDown = isDown;
}

static void winHandleMessages(
    slurp::KeyboardState& outKeyboardState,
    slurp::MouseState& outMouseState,
    const slurp::Vec2<int>& screenDimensions
) {
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                WPARAM virtualKeyCode = message.wParam;
                bool wasDown = (1 << 30) & message.lParam;
                bool isDown = ((1 << 31) & message.lParam) == 0;

                if (KeyboardWinCodeToSlurpCode.count(virtualKeyCode) > 0) {
                    slurp::KeyboardCode code = KeyboardWinCodeToSlurpCode.at(virtualKeyCode);
                    // TODO: change transition count computation once we poll multiple times per frame
                    slurp::DigitalInputState& inputState = outKeyboardState.state[code];
                    inputState.transitionCount = wasDown != isDown ? 1 : 0;
                    // TODO: do we need to clear this every frame?
                    inputState.isDown = isDown;
                } else { logging::error("Windows keyboard code not registered.\n"); }
            }
            break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP: {
                bool isDown = message.wParam & MK_LBUTTON;
                updateMouseButtonState(outMouseState, slurp::MouseCode::LeftClick, isDown);
            }
            break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                bool isDown = message.wParam & MK_MBUTTON;
                updateMouseButtonState(outMouseState, slurp::MouseCode::MiddleClick, isDown);
            }
            break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP: {
                bool isDown = message.wParam & MK_RBUTTON;
                updateMouseButtonState(outMouseState, slurp::MouseCode::RightClick, isDown);
            }
            break;
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP: {
                bool isXButton1Down = message.wParam & MK_XBUTTON1;
                updateMouseButtonState(outMouseState, slurp::MouseCode::Button1, isXButton1Down);

                bool isXButton2Down = message.wParam & MK_XBUTTON2;
                updateMouseButtonState(outMouseState, slurp::MouseCode::Button2, isXButton2Down);
            }
            break;
            case WM_MOUSEMOVE: {
                slurp::Vec2 mouseScreenPosition = {
                    GET_X_LPARAM(message.lParam),
                    screenDimensions.height - GET_Y_LPARAM(message.lParam)
                };
                // TODO: maybe this should be cached and updated whenever the screen dimensions (infrequently) update
                slurp::Mat32<float> screenToWorldMatrix = {
                    {static_cast<float>(CAMERA_WORLD_WIDTH) / screenDimensions.width, 0.f},
                    {0.f, static_cast<float>(CAMERA_WORLD_HEIGHT) / screenDimensions.height},
                    {-CAMERA_WORLD_WIDTH_MAX, -CAMERA_WORLD_HEIGHT_MAX},
                };
                outMouseState.position = mouseScreenPosition * screenToWorldMatrix;
            }
            break;
            case WM_QUIT: { GlobalRunning = false; }
            break;
            default:
                TranslateMessage(&message);
                DispatchMessageA(&message);
        }
    }
}

#define X_INPUT_GET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_STATE* pState)

typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XInputGetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }

static x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)

typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }

static x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

static void winLoadXInput() {
    HMODULE xInputLib = LoadLibraryA("xinput1_4.dll");
    if (!xInputLib) {
        logging::warn("Could not load xinput 1.4, falling back to xinput 1.3");
        xInputLib = LoadLibraryA("xinput1_3.dll");
    }
    if (xInputLib) {
        XInputGetState = reinterpret_cast<x_input_get_state*>(GetProcAddress(xInputLib, "XInputGetState"));
        XInputSetState = reinterpret_cast<x_input_set_state*>(GetProcAddress(xInputLib, "XInputSetState"));
    } else {
        logging::error("Could not load xinput");
    }
}

#define XINPUT_STICK_MAG_POS 32767
#define XINPUT_STICK_MAG_NEG 32768
#define XINPUT_TRIGGER_MAG 255
#define XINPUT_VIBRATION_MAG 65535

static float winGetNormalizedStickValue(int16_t stickValue, int16_t deadZone) {
    if (abs(stickValue) < deadZone) { return 0.f; }
    if (stickValue > 0) { return static_cast<float>(stickValue) / XINPUT_STICK_MAG_POS; }
    return static_cast<float>(stickValue) / XINPUT_STICK_MAG_NEG;
}

static float winGetNormalizedTriggerValue(uint8_t triggerValue) {
    if (triggerValue < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) { return 0.f; }
    return static_cast<float>(triggerValue) / XINPUT_TRIGGER_MAG;
}

static void winHandleGamepadInput(slurp::GamepadState* gamepadStates) {
    for (DWORD gamepadIndex = 0; gamepadIndex < XUSER_MAX_COUNT; gamepadIndex++) {
        XINPUT_STATE xInputState;
        DWORD result = XInputGetState(gamepadIndex, &xInputState);
        if (result == ERROR_SUCCESS) {
            slurp::GamepadState* gamepadState = &gamepadStates[gamepadIndex];
            gamepadState->isConnected = true;

            XINPUT_GAMEPAD gamepad = xInputState.Gamepad;
            for (std::pair<XInputCode, slurp::GamepadCode> entry: GamepadWinCodeToSlurpCode) {
                XInputCode xInputCode = entry.first;
                bool isDown = gamepad.wButtons & xInputCode;

                slurp::GamepadCode gamepadCode = entry.second;
                slurp::DigitalInputState* inputState = &gamepadState->state[gamepadCode];
                inputState->transitionCount = inputState->isDown != isDown ? 1 : 0;
                inputState->isDown = isDown;
            }

            float leftStickXNormalized = winGetNormalizedStickValue(
                gamepad.sThumbLX,
                XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
            );
            float leftStickYNormalized = winGetNormalizedStickValue(
                gamepad.sThumbLY,
                XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
            );
            slurp::AnalogStickInputState& leftStickState = gamepadState->leftStick;
            leftStickState.start = leftStickState.end;
            leftStickState.end.x = leftStickXNormalized;
            leftStickState.end.y = leftStickYNormalized;

            float rightStickXNormalized = winGetNormalizedStickValue(
                gamepad.sThumbRX,
                XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
            );
            float rightStickYNormalized = winGetNormalizedStickValue(
                gamepad.sThumbRY,
                XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
            );
            slurp::AnalogStickInputState& rightStickState = gamepadState->rightStick;
            rightStickState.start = rightStickState.end;
            rightStickState.end.x = rightStickXNormalized;
            rightStickState.end.y = rightStickYNormalized;

            float leftTriggerNormalized = winGetNormalizedTriggerValue(gamepad.bLeftTrigger);
            slurp::AnalogTriggerInputState& leftTriggerState = gamepadState->leftTrigger;
            leftTriggerState.start = leftTriggerState.end;
            leftTriggerState.end = leftTriggerNormalized;

            float rightTriggerNormalized = winGetNormalizedTriggerValue(gamepad.bRightTrigger);
            slurp::AnalogTriggerInputState& rightTriggerState = gamepadState->rightTrigger;
            rightTriggerState.start = rightTriggerState.end;
            rightTriggerState.end = rightTriggerNormalized;
        } else {
            // Gamepad is not connected
            // TODO: log
        }
    }
};

#define DIRECT_SOUND_CREATE(fnName) HRESULT WINAPI fnName(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS, LPUNKNOWN pUnkOuter)

typedef DIRECT_SOUND_CREATE(direct_sound_create);

static void winInitDirectSound(HWND windowHandle) {
    HMODULE dSoundLib = LoadLibraryA("dsound.dll");
    if (dSoundLib) {
        GlobalAudioBuffer.samplesPerSec = AUDIO_SAMPLES_PER_SECOND;
        GlobalAudioBuffer.bytesPerSample = TOTAL_AUDIO_SAMPLE_SIZE;
        GlobalAudioBuffer.bufferSizeBytes =
                AUDIO_BUFFER_SECONDS * GlobalAudioBuffer.samplesPerSec * GlobalAudioBuffer.bytesPerSample;
        // NOTE: tuned to the max latency between writeCursor readings.
        GlobalAudioBuffer.writeAheadSampleCount = static_cast<int>(
            GlobalAudioBuffer.samplesPerSec * AUDIO_BUFFER_WRITE_AHEAD_SECONDS
        );

        direct_sound_create* directSoundCreate = reinterpret_cast<direct_sound_create*>(GetProcAddress(
            dSoundLib,
            "DirectSoundCreate8"
        ));
        LPDIRECTSOUND8 directSound;
        if (directSoundCreate && SUCCEEDED(directSoundCreate(nullptr, &directSound, nullptr))) {
            directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY);

            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            waveFormat.nChannels = NUM_AUDIO_CHANNELS;
            waveFormat.nSamplesPerSec = GlobalAudioBuffer.samplesPerSec;
            waveFormat.wBitsPerSample = PER_CHANNEL_AUDIO_SAMPLE_SIZE_BITS;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / BITS_PER_BYTE;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

            ASSERT((waveFormat.wBitsPerSample % BITS_PER_BYTE) == 0);

            WAVEFORMATEXTENSIBLE waveFormatExtensible = {
                waveFormat,
                waveFormat.wBitsPerSample,
#if NUM_AUDIO_CHANNELS == 2
                SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
#else
            SPEAKER_ALL,
#endif
                KSDATAFORMAT_SUBTYPE_PCM,
            };

            // Primary Buffer
            DSBUFFERDESC dsBufferDescription = {};
            dsBufferDescription.dwSize = sizeof(dsBufferDescription);
            dsBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            LPDIRECTSOUNDBUFFER dsPrimaryBuffer;
            if (SUCCEEDED(directSound->CreateSoundBuffer(&dsBufferDescription, &dsPrimaryBuffer, nullptr))) {
                if (SUCCEEDED(dsPrimaryBuffer->SetFormat(reinterpret_cast<LPCWAVEFORMATEX>(&waveFormatExtensible)))) {
                    logging::info("Primary audio buffer created.");
                } else {
                    logging::error("Failed to create primar audio buffer.");
                }
            }

            // Secondary Buffer
            DSBUFFERDESC dsSecBufferDescription = {};
            dsSecBufferDescription.dwSize = sizeof(dsSecBufferDescription);
            dsSecBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
            dsSecBufferDescription.dwBufferBytes = GlobalAudioBuffer.bufferSizeBytes;
            dsSecBufferDescription.lpwfxFormat = reinterpret_cast<LPWAVEFORMATEX>(&waveFormatExtensible);
            if (SUCCEEDED(
                directSound->CreateSoundBuffer(
                    &dsSecBufferDescription,
                    &GlobalAudioBuffer.buffer,
                    nullptr
                )
            )) {
                logging::info("Secondary audio buffer created.");
            } else {
                logging::error("Failed to create secondary audio buffer.");
            }
        }
    }
}

static DWORD winBufferAudio(DWORD lockCursor, DWORD targetCursor) {
    DWORD numBytesToWrite = 0;
    if (lockCursor > targetCursor) {
        numBytesToWrite = GlobalAudioBuffer.bufferSizeBytes - lockCursor + targetCursor;
    } else {
        numBytesToWrite = targetCursor - lockCursor;
    }

    if (numBytesToWrite == 0) { return 0; }

    void* audioRegion1Ptr;
    DWORD audioRegion1Bytes;
    void* audioRegion2Ptr;
    DWORD audioRegion2Bytes;
    if (!SUCCEEDED(
        GlobalAudioBuffer.buffer->Lock(
            lockCursor,
            numBytesToWrite,
            &audioRegion1Ptr,
            &audioRegion1Bytes,
            &audioRegion2Ptr,
            &audioRegion2Bytes,
            0
        )
    )) {
        logging::error("Audio buffer lock failed.\n");
        return 0;
    }

    audio::AudioBuffer region1Buffer = {};
    region1Buffer.samples = static_cast<audio::StereoAudioSample*>(audioRegion1Ptr);
    region1Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region1Buffer.numSamplesToWrite = audioRegion1Bytes / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.bufferAudio(region1Buffer);

    audio::AudioBuffer region2Buffer = {};
    region2Buffer.samples = static_cast<audio::StereoAudioSample*>(audioRegion2Ptr);
    region2Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region2Buffer.numSamplesToWrite = audioRegion2Bytes / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.bufferAudio(region2Buffer);

    GlobalAudioBuffer.buffer->Unlock(
        audioRegion1Ptr,
        audioRegion1Bytes,
        audioRegion2Ptr,
        audioRegion2Bytes
    );
    return numBytesToWrite;
}

static DWORD winGetMonitorRefreshRate() {
    DEVMODEA devMode = {};
    devMode.dmSize = sizeof(devMode);
    if (!EnumDisplaySettingsExA(
        nullptr,
        ENUM_CURRENT_SETTINGS,
        &devMode,
        EDS_RAWMODE
    )) {
        logging::error("Could not fetch monitor refresh rate.\n");
        return DEFAULT_MONITOR_REFRESH_RATE;
    }
    return devMode.dmDisplayFrequency;
}

static void winAllocateGameMemory(memory::MemoryArena& outPermanentMemory, memory::MemoryArena& outTransientMemory) {
    size_t permanentMemorySizeBytes = PERMANENT_ARENA_SIZE;
    size_t transientMemorySizeBytes = TRANSIENT_ARENA_SIZE;
#if DEBUG
    void* baseAddress = (void*) terabytes(1);
#else
	void* baseAddress = nullptr;
#endif
    size_t size = permanentMemorySizeBytes + transientMemorySizeBytes;
    types::byte* memory = static_cast<types::byte*>(VirtualAlloc(
        baseAddress,
        size,
        MEM_RESERVE | MEM_COMMIT,
        // TODO: could we use MEM_LARGE_PAGES to alleviate TLB
        PAGE_READWRITE
    ));
    memory::MemoryArena fullMemory("Full Memory", {memory, size});
    outPermanentMemory = fullMemory.allocateSubArena("Permanent Memory", permanentMemorySizeBytes);
    outTransientMemory = fullMemory.allocateSubArena("Transient Memory", transientMemorySizeBytes);
}

static float winGetFrameMillis(
    WinTimingInfo startTimingInfo,
    LARGE_INTEGER& outPerformanceCounterEnd
) {
    QueryPerformanceCounter(&outPerformanceCounterEnd);
    return (outPerformanceCounterEnd.QuadPart - startTimingInfo.performanceCounter) * 1000.f /
           startTimingInfo.performanceCounterFrequency;
}

#define SLEEP_STALL_MIN_LEFTOVER_MS 0.5f

static void winStallFrameToTarget(
    float targetMillisPerFrame,
    WinTimingInfo startTimingInfo,
    bool isSleepGranular
) {
    LARGE_INTEGER performanceCounterEnd;
    float frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    if (frameMillis >= targetMillisPerFrame) {
        // logging::error("Frame too slow. Target frame rate missed.\n");
        return;
    }
    if (isSleepGranular) {
        float stallMs = targetMillisPerFrame - frameMillis;
        DWORD sleepMs = static_cast<DWORD>(stallMs);
        if (sleepMs > 1 && (stallMs - sleepMs) > SLEEP_STALL_MIN_LEFTOVER_MS) { sleepMs -= 1; }
        if (sleepMs > 0) { Sleep(sleepMs); }
    }
    while (frameMillis < targetMillisPerFrame) {
        frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    }
}

static void winCaptureAndLogPerformance(
    uint64_t& startProcessorCycle,
    WinTimingInfo& startTimingInfo
) {
    uint64_t processorCycleEnd = __rdtsc();
    LARGE_INTEGER performanceCounterEnd;

    float frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    int fps = static_cast<int>(1000 / frameMillis);
    int frameProcessorMCycles = static_cast<int>((processorCycleEnd - startProcessorCycle) / 1000 / 1000);

#if VERBOSE_LOGGING
    logging::info(std::format("Frame: {:.2f}ms {}fps {} processor mega-cycles", frameMillis, fps, frameProcessorMCycles));
#endif

    startProcessorCycle = processorCycleEnd;
    startTimingInfo.performanceCounter = performanceCounterEnd.QuadPart;
}

template<typename T>
static void winLoadLibFn(T*& out, LPCSTR fnName, T* stubFn, const HMODULE& lib) {
    out = reinterpret_cast<T*>(
        GetProcAddress(lib, fnName)
    );
    if (!out) {
        ASSERT_LOG(out, std::format("Failed to load lib function: {}.", fnName));
        out = stubFn;
    }
}

static void winLoadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath) {
    if (!CopyFileA(dllFilePath, dllLoadFilePath, false)) {
        logging::error("Failed to copy SlurpEngine.dll.");
    }
    GlobalSlurpLib = LoadLibraryA(dllLoadFilePath);
    if (!GlobalSlurpLib) {
        logging::error("Failed to load SlurpEngineLoad.dll.");
    } else {
        winLoadLibFn<slurp::dyn_init>(
            GlobalSlurpDll.init,
            "init",
            slurp::stub_init,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_frameStart>(
            GlobalSlurpDll.frameStart,
            "frameStart",
            slurp::stub_frameStart,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_handleInput>(
            GlobalSlurpDll.handleInput,
            "handleInput",
            slurp::stub_handleInput,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_bufferAudio>(
            GlobalSlurpDll.bufferAudio,
            "bufferAudio",
            slurp::stub_bufferAudio,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_updateAndRender>(
            GlobalSlurpDll.updateAndRender,
            "updateAndRender",
            slurp::stub_updateAndRender,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_frameEnd>(
            GlobalSlurpDll.frameEnd,
            "frameEnd",
            slurp::stub_frameEnd,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_shutdown>(
            GlobalSlurpDll.shutdown,
            "shutdown",
            slurp::stub_shutdown,
            GlobalSlurpLib
        );
    }
}

static void winUnloadSlurpLib() {
    if (!GlobalSlurpLib) {
        return;
    }
    GlobalSlurpDll.shutdown();
    if (!FreeLibrary(GlobalSlurpLib)) {
        logging::error("Failed to unload Slurp lib.");
    }
    GlobalSlurpDll = slurp::SlurpDll();
}

static void winTryReloadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath, bool isInitialized) {
    HANDLE dllMarkerFileHandle = CreateFileA(
        SLURP_DLL_MARKER_FILE_NAME,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    static FILETIME previousWriteTime;
    FILETIME writeTime;
    FILETIME _;
    if (!GetFileTime(dllMarkerFileHandle, &_, &_, &writeTime)) {
        logging::error("Failed to get SlurpEngine.dll file time.");
        return;
    }
    CloseHandle(dllMarkerFileHandle);
    if (CompareFileTime(&writeTime, &previousWriteTime) == 0) { return; }

    previousWriteTime = writeTime;
    winUnloadSlurpLib();
    winLoadSlurpLib(dllFilePath, dllLoadFilePath);
    GlobalSlurpDll.init(
        GlobalPermanentMemory,
        GlobalTransientMemory,
        GlobalPlatformDll,
        GlobalRenderApi,
        isInitialized
    );
}

static std::string getLocalFilePath(LPCSTR filename) {
    char exeDirPath[MAX_PATH];
    GetModuleFileNameA(nullptr, exeDirPath, MAX_PATH);
    PathRemoveFileSpecA(exeDirPath);
    std::string exeDirPathStr = std::string(exeDirPath);
    return exeDirPathStr + "\\" + filename;
}

#if DEBUG
PLATFORM_DEBUG_READ_FILE(platform::DEBUG_readFile) {
    platform::DEBUG_FileReadResult result = {};
    HANDLE fileHandle = CreateFileA(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        logging::error("Invalid file handle.");
        return result;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(
        fileHandle,
        &fileSize
    );

    ASSERT(fileSize.QuadPart < gigabytes(4));
    DWORD fileSizeTruncated = static_cast<uint32_t>(fileSize.QuadPart);
    void* buffer = VirtualAlloc(
        nullptr,
        fileSizeTruncated,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    DWORD bytesRead;
    bool success = ReadFile(
        fileHandle,
        buffer,
        fileSizeTruncated,
        &bytesRead,
        nullptr
    );
    CloseHandle(fileHandle);

    if (!success || bytesRead != fileSizeTruncated) {
        logging::error("Could not read file.");
        platform::DEBUG_freeMemory(buffer);
        return result;
    }

    result.fileContents = buffer;
    result.sizeBytes = fileSizeTruncated;
    return result;
}

PLATFORM_DEBUG_WRITE_FILE(platform::DEBUG_writeFile) {
    HANDLE fileHandle = CreateFileA(
        fileName,
        GENERIC_WRITE,
        FILE_SHARE_DELETE,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        logging::error("Invalid file handle.");
        return false;
    }

    DWORD bytesWritten;
    bool success = WriteFile(
        fileHandle,
        fileContents,
        sizeBytes,
        &bytesWritten,
        nullptr
    );
    CloseHandle(fileHandle);

    if (!success || bytesWritten != sizeBytes) {
        logging::error("Could not write file.");
        return false;
    }
    return true;
}

PLATFORM_DEBUG_FREE_MEMORY(platform::DEBUG_freeMemory) { if (memory) { VirtualFree(memory, 0, MEM_RELEASE); } }

PLATFORM_DEBUG_TOGGLE_PAUSE(platform::DEBUG_togglePause) {
    GlobalRecordingState.isPaused = !GlobalRecordingState.isPaused;
}

PLATFORM_DEBUG_BEGIN_RECORDING(platform::DEBUG_beginRecording) {
    GlobalRecordingState.recordingFileHandle = CreateFileA(
        getLocalFilePath(RECORDING_FILE_NAME).c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    DWORD _;
    memory::MemoryBlock recordingMemory = GlobalPermanentMemory.getMemoryBlock();
    WriteFile(
        GlobalRecordingState.recordingFileHandle,
        recordingMemory.memory,
        recordingMemory.size,
        &_,
        nullptr
    );
    GlobalRecordingState.isRecording = true;
}

template<typename T>
static void winRecordStateMap(std::unordered_map<T, slurp::DigitalInputState> stateMap) {
    DWORD _;
    size_t numStates = stateMap.size();
    WriteFile(
        GlobalRecordingState.recordingFileHandle,
        &numStates,
        sizeof(numStates),
        &_,
        nullptr
    );
    for (const std::pair<const T, slurp::DigitalInputState>& entry: stateMap) {
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &entry,
            sizeof(entry),
            &_,
            nullptr
        );
    }
}

static void winRecordInput(
    const slurp::MouseState& mouseState,
    const slurp::KeyboardState& keyboardState,
    slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS]
) {
    DWORD _;
    // record mouse input
    WriteFile(
        GlobalRecordingState.recordingFileHandle,
        &mouseState.position,
        sizeof(mouseState.position),
        &_,
        nullptr
    );
    winRecordStateMap(mouseState.state);

    // record keyboard input
    winRecordStateMap(keyboardState.state);

    // record gamepad input
    for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
        slurp::GamepadState& gamepadState = gamepadStates[gamepadIndex];
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.isConnected,
            sizeof(gamepadState.isConnected),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.leftStick,
            sizeof(gamepadState.leftStick),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.rightStick,
            sizeof(gamepadState.rightStick),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.leftTrigger,
            sizeof(gamepadState.leftTrigger),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.rightTrigger,
            sizeof(gamepadState.rightTrigger),
            &_,
            nullptr
        );
        winRecordStateMap(gamepadState.state);
    }
}

PLATFORM_DEBUG_END_RECORDING(platform::DEBUG_endRecording) {
    GlobalRecordingState.isRecording = false;
    CloseHandle(GlobalRecordingState.recordingFileHandle);
}

PLATFORM_DEBUG_BEGIN_PLAYBACK(platform::DEBUG_beginPlayback) {
    GlobalRecordingState.recordingFileHandle = CreateFileA(
        getLocalFilePath(RECORDING_FILE_NAME).c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    DWORD _;
    memory::MemoryBlock recordingMemory = GlobalPermanentMemory.getMemoryBlock();
    ReadFile(
        GlobalRecordingState.recordingFileHandle,
        recordingMemory.memory,
        recordingMemory.size,
        &_,
        nullptr
    );
    GlobalRecordingState.isPlayingBack = true;
    GlobalRecordingState.onPlaybackEnd = onPlaybackEnd;
}

template<typename T>
static void winReadInputStateMap(std::unordered_map<T, slurp::DigitalInputState>& outStateMap) {
    DWORD _;
    size_t numStates = 0;
    ReadFile(
        GlobalRecordingState.recordingFileHandle,
        &numStates,
        sizeof(numStates),
        &_,
        nullptr
    );
    outStateMap.clear();
    for (size_t i = 0; i < numStates; i++) {
        std::pair<const T, slurp::DigitalInputState> entry;
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &entry,
            sizeof(entry),
            &_,
            nullptr
        );
        outStateMap.insert(entry);
    }
}

static void winReadInputRecording(
    slurp::MouseState& outMouseState,
    slurp::KeyboardState& outKeyboardState,
    slurp::GamepadState outGamepadStates[MAX_NUM_GAMEPADS]
) {
    DWORD bytesRead;
    // read mouse input
    ReadFile(
        GlobalRecordingState.recordingFileHandle,
        &outMouseState.position,
        sizeof(outMouseState.position),
        &bytesRead,
        nullptr
    );
    if (bytesRead == 0) {
        GlobalRecordingState.isPlayingBack = false;
        CloseHandle(GlobalRecordingState.recordingFileHandle);
        GlobalRecordingState.onPlaybackEnd();
    }

    DWORD _;
    winReadInputStateMap(outMouseState.state);

    // read keyboard input
    winReadInputStateMap(outKeyboardState.state);

    // read gamepad input
    for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
        slurp::GamepadState& outGamepadState = outGamepadStates[gamepadIndex];
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.isConnected,
            sizeof(outGamepadState.isConnected),
            &_,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.leftStick,
            sizeof(outGamepadState.leftStick),
            &_,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.rightStick,
            sizeof(outGamepadState.rightStick),
            &_,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.leftTrigger,
            sizeof(outGamepadState.leftTrigger),
            &_,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.rightTrigger,
            sizeof(outGamepadState.rightTrigger),
            &_,
            nullptr
        );
        winReadInputStateMap(outGamepadState.state);
    }
}
#endif

PLATFORM_VIBRATE_GAMEPAD(platform::vibrateGamepad) {
    uint16_t leftMotorSpeedRaw = static_cast<uint16_t>(leftMotorSpeed * XINPUT_VIBRATION_MAG);
    uint16_t rightMotorSpeedRaw = static_cast<uint16_t>(rightMotorSpeed * XINPUT_VIBRATION_MAG);
    XINPUT_VIBRATION vibration{
        leftMotorSpeedRaw,
        rightMotorSpeedRaw,
    };
    XInputSetState(gamepadIndex, &vibration);
}

PLATFORM_SHUTDOWN(platform::shutdown) { GlobalRunning = false; }

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
    renderApi.setBackgroundColor = open_gl::setBackgroundColor;
    renderApi.createTexture = open_gl::createTexture;
    renderApi.createShaderProgram = open_gl::createShaderProgram;
    renderApi.genVertexArrayBuffer = open_gl::genVertexArrayBuffer;
    renderApi.genElementArrayBuffer = open_gl::genElementArrayBuffer;
    renderApi.drawVertexArray = open_gl::drawVertexArray;
    renderApi.drawElementArray = open_gl::drawElementArray;
    renderApi.drawLine = open_gl::drawLine;
    renderApi.deleteResources = open_gl::deleteResources;
    return renderApi;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow
) {
#if RENDER_API == OPEN_GL
    open_gl::OpenGLRenderWindow renderWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, WINDOW_TITLE, FULLSCREEN);
    if (!renderWindow.isValid()) { return 1; }
#endif
    HWND windowHandle = renderWindow.getWin32Handle();

    GlobalRunning = true;

    // winLoadSlurpLib(dllFilePath, dllLoadFilePath);

    GlobalPlatformDll = loadPlatformDll();
    GlobalRenderApi = loadRenderApi();
    winAllocateGameMemory(GlobalPermanentMemory, GlobalTransientMemory);
    std::string dllFilePathStr = getLocalFilePath(SLURP_DLL_FILE_NAME);
    const char* dllFilePath = dllFilePathStr.c_str();
    std::string dllLoadFilePathStr = getLocalFilePath(SLURP_LOAD_DLL_FILE_NAME);
    const char* dllLoadFilePath = dllLoadFilePathStr.c_str();
    winTryReloadSlurpLib(dllFilePath, dllLoadFilePath, false);
    winLoadXInput();

    bool isSleepGranular = timeBeginPeriod(1) == TIMERR_NOERROR;
#if DEBUG
    DWORD targetFramesPerSecond = DEBUG_MONITOR_REFRESH_RATE;
#else
    DWORD targetFramesPerSecond = winGetMonitorRefreshRate();
#endif
    float targetSecondsPerFrame = 1.f / targetFramesPerSecond;
    float targetMillisPerFrame = targetSecondsPerFrame * 1000.f;

    slurp::MouseState mouseState;
    slurp::KeyboardState keyboardState;
    slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS];

#if !DEBUG_SHOW_MOUSE_CURSOR
    ShowCursor(false);
#endif

    // TODO: migrate to the newer XAudio2
    // TODO: could dynamically track max writeCursor diff and update GlobalAudioBuffer.writeAheadSampleCount
    winInitDirectSound(windowHandle);
    DWORD playCursor = 0;
    DWORD writeCursor = 0;
    DWORD lockCursor = 0;
    GlobalAudioBuffer.buffer->Play(0, 0, DSBPLAY_LOOPING);

    uint64_t startProcessorCycle = __rdtsc();
    LARGE_INTEGER startPerformanceCounter;
    QueryPerformanceCounter(&startPerformanceCounter);
    LARGE_INTEGER performanceCounterFrequency;
    QueryPerformanceFrequency(&performanceCounterFrequency);
    WinTimingInfo startTimingInfo = {
        startPerformanceCounter.QuadPart,
        performanceCounterFrequency.QuadPart
    };

    while (GlobalRunning) {
        winTryReloadSlurpLib(dllFilePath, dllLoadFilePath, true);

        GlobalSlurpDll.frameStart();

        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry: keyboardState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        for (std::pair<const slurp::MouseCode, slurp::DigitalInputState>& entry: mouseState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        winHandleMessages(keyboardState, mouseState, renderWindow.getDimensions());
        winHandleGamepadInput(gamepadStates);
#if DEBUG
        if (GlobalRecordingState.isRecording) { winRecordInput(mouseState, keyboardState, gamepadStates); }
        if (GlobalRecordingState.isPlayingBack) { winReadInputRecording(mouseState, keyboardState, gamepadStates); }
#endif
        GlobalSlurpDll.handleInput(mouseState, keyboardState, gamepadStates);

#if DEBUG
        if (GlobalRecordingState.isPaused) { continue; }
#endif

        GlobalSlurpDll.updateAndRender(targetSecondsPerFrame);

        if (GlobalAudioBuffer.buffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK) {
            logging::error("Get audio buffer position failed.\n");
        }
        // NOTE: We always set our targetCursor to right after the write cursor,
        // this means audio is played as soon as possible.
        // TODO: if the sound card has very low latency, we could play audio earlier than the frame flip
        DWORD targetCursor = (writeCursor +
                              GlobalAudioBuffer.writeAheadSampleCount * GlobalAudioBuffer.bytesPerSample
                             ) % GlobalAudioBuffer.bufferSizeBytes;
        DWORD numBytesWritten = winBufferAudio(lockCursor, targetCursor);
        lockCursor = (lockCursor + numBytesWritten) % GlobalAudioBuffer.bufferSizeBytes;

        winStallFrameToTarget(targetMillisPerFrame, startTimingInfo, isSleepGranular);
        winCaptureAndLogPerformance(startProcessorCycle, startTimingInfo);

        renderWindow.flip();
        if (renderWindow.shouldTerminate()) {
            GlobalRunning = false;
        }

        GlobalSlurpDll.frameEnd();
    }

    if (isSleepGranular) { timeEndPeriod(1); }
    return 0;
}
