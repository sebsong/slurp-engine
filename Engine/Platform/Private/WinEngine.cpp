#include "Platform.h"
#include "WinEngine.h"
#include "Debug.h"

#include <iostream>
#include <string>
#include <format>
#include <shlwapi.h>
#include <windowsx.h>

#define kilobytes(n) ((int64_t)n * 1024)
#define megabytes(n) (kilobytes(n) * 1024)
#define gigabytes(n) (megabytes(n) * 1024)
#define terabytes(n) (gigabytes(n) * 1024)

#if DEBUG
#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720
// #define DISPLAY_WIDTH 640
// #define DISPLAY_HEIGHT 360
#else
#define DISPLAY_WIDTH 2560
#define DISPLAY_HEIGHT 1440
#endif
#define FIT_TO_SCREEN 1
#define DEFAULT_MONITOR_REFRESH_RATE 144
#define DEBUG_MONITOR_REFRESH_RATE 120

#define NUM_AUDIO_CHANNELS 1
#define AUDIO_SAMPLES_PER_SECOND 44100
#define AUDIO_WRITE_AHEAD_SECONDS 0.01

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";
static const LPCSTR SLURP_DLL_FILE_NAME = "SlurpEngine.dll";
static const LPCSTR SLURP_LOAD_DLL_FILE_NAME = "SlurpEngineLoad.dll";
static const LPCSTR RECORDING_FILE_NAME = "SlurpRecording.rec";

static bool GlobalRunning;

static WinGraphicsBuffer GlobalGraphicsBuffer;

static WinAudioBuffer GlobalAudioBuffer;

static platform::PlatformDll GlobalPlatformDll;
static platform::GameMemory GlobalGameMemory;
static slurp::SlurpDll GlobalSlurpDll;
static HMODULE GlobalSlurpLib;

#if DEBUG
static WinRecordingState GlobalRecordingState;
#endif

static WinScreenDimensions winGetScreenDimensions(HWND windowHandle) {
    RECT rect;
    GetClientRect(windowHandle, &rect);
    return WinScreenDimensions
    {
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
    };
};

static void winResizeDIBSection(WinGraphicsBuffer* outBuffer, int width, int height) {
    if (outBuffer->memory) { VirtualFree(outBuffer->memory, 0, MEM_RELEASE); }

    int bytesPerPixel = 4;
    outBuffer->widthPixels = width;
    outBuffer->heightPixels = height;
    outBuffer->bytesPerPixel = bytesPerPixel;
    outBuffer->pitchBytes = width * bytesPerPixel;

    outBuffer->info.bmiHeader.biSize = sizeof(outBuffer->info.bmiHeader);
    outBuffer->info.bmiHeader.biWidth = outBuffer->widthPixels;
    outBuffer->info.bmiHeader.biHeight = -outBuffer->heightPixels;
    outBuffer->info.bmiHeader.biPlanes = 1;
    outBuffer->info.bmiHeader.biBitCount = static_cast<WORD>(bytesPerPixel * 8);
    outBuffer->info.bmiHeader.biCompression = BI_RGB;

    int bitmapSizeBytes = outBuffer->widthPixels * outBuffer->heightPixels * bytesPerPixel;
    outBuffer->memory = VirtualAlloc(
        nullptr,
        bitmapSizeBytes,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
}

static void winUpdateWindow(
    HDC deviceContextHandle,
    const WinGraphicsBuffer& buffer,
    int screenWidth,
    int screenHeight
) {
#if !FIT_TO_SCREEN
    PatBlt(
        deviceContextHandle,
        0,
        buffer.heightPixels,
        screenWidth,
        screenHeight - buffer.heightPixels,
        BLACKNESS
    );
    PatBlt(
        deviceContextHandle,
        buffer.widthPixels,
        0,
        screenWidth - buffer.widthPixels,
        buffer.heightPixels,
        BLACKNESS
    );
#endif

    // TODO: aspect ratio correction
    StretchDIBits(
        deviceContextHandle,
#if FIT_TO_SCREEN
        0,
        0,
        screenWidth,
        screenHeight,
#else
        0, 0, buffer.widthPixels, buffer.heightPixels, // for pixel perfect scaling
#endif
        0,
        0,
        buffer.widthPixels,
        buffer.heightPixels,
        buffer.memory,
        &buffer.info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
};

static void winPaint(HWND windowHandle, const WinGraphicsBuffer buffer) {
    PAINTSTRUCT paintStruct;
    HDC deviceContext = BeginPaint(windowHandle, &paintStruct);
    WinScreenDimensions dimensions = winGetScreenDimensions(windowHandle);
    winUpdateWindow(deviceContext, buffer, dimensions.width, dimensions.height);
    EndPaint(windowHandle, &paintStruct);
    ReleaseDC(windowHandle, deviceContext);
}

static LRESULT CALLBACK winMessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (message) {
        case WM_ACTIVATEAPP: {}
        break;
        case WM_SIZE: {}
        break;
        case WM_DESTROY:
        case WM_CLOSE: { GlobalRunning = false; }
        break;
        case WM_PAINT: { winPaint(windowHandle, GlobalGraphicsBuffer); }
        break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: { assert(!"Keyboard event should not be handled in Windows handler."); }
        break;
        default: { result = DefWindowProcA(windowHandle, message, wParam, lParam); }
        break;
    }

    return result;
};

static void updateMouseButtonState(slurp::MouseState& outMouseState, slurp::MouseCode mouseCode, bool isDown) {
    slurp::DigitalInputState& inputState = outMouseState.state[mouseCode];
    inputState.transitionCount = inputState.isDown != isDown ? 1 : 0;;
    inputState.isDown = isDown;
}

static void winHandleMessages(
    slurp::KeyboardState& outKeyboardState,
    slurp::MouseState& outMouseState,
    const WinScreenDimensions& screenDimensions,
    const WinGraphicsBuffer& buffer
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
                } else { OutputDebugStringA("Windows keyboard code not registered.\n"); }
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
                int xPosition = GET_X_LPARAM(message.lParam);
                int yPosition = GET_Y_LPARAM(message.lParam);
#if FIT_TO_SCREEN
                if (screenDimensions.width) {
                    xPosition *= (static_cast<float>(buffer.widthPixels) / screenDimensions.width);
                }
                if (screenDimensions.height) {
                    yPosition *= (static_cast<float>(buffer.heightPixels) / screenDimensions.height);
                }
#endif
                outMouseState.position = {static_cast<float>(xPosition), static_cast<float>(yPosition)};
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
        // TODO: log
        xInputLib = LoadLibraryA("xinput1_3.dll");
    }
    if (xInputLib) {
        XInputGetState = reinterpret_cast<x_input_get_state*>(GetProcAddress(xInputLib, "XInputGetState"));
        XInputSetState = reinterpret_cast<x_input_set_state*>(GetProcAddress(xInputLib, "XInputSetState"));
    } else {
        // TODO: log
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

#define DIRECT_SOUND_CREATE(fnName) HRESULT WINAPI fnName(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)

typedef DIRECT_SOUND_CREATE(direct_sound_create);

static void winInitDirectSound(HWND windowHandle) {
    HMODULE dSoundLib = LoadLibraryA("dsound.dll");
    if (dSoundLib) {
        GlobalAudioBuffer.samplesPerSec = AUDIO_SAMPLES_PER_SECOND;
        GlobalAudioBuffer.bytesPerSample = sizeof(audio::audio_sample_t);
        GlobalAudioBuffer.bufferSizeBytes = GlobalAudioBuffer.samplesPerSec * GlobalAudioBuffer.bytesPerSample;
        // NOTE: tuned to the max latency between writeCursor readings.
        GlobalAudioBuffer.writeAheadSampleCount = static_cast<int>(
            GlobalAudioBuffer.samplesPerSec * AUDIO_WRITE_AHEAD_SECONDS);

        direct_sound_create* directSoundCreate = reinterpret_cast<direct_sound_create*>(GetProcAddress(
            dSoundLib,
            "DirectSoundCreate"
        ));
        LPDIRECTSOUND directSound;
        if (directSoundCreate && SUCCEEDED(directSoundCreate(nullptr, &directSound, nullptr))) {
            directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY);

            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = NUM_AUDIO_CHANNELS;
            waveFormat.nSamplesPerSec = GlobalAudioBuffer.samplesPerSec;
            waveFormat.wBitsPerSample = sizeof(audio::audio_sample_t) * 8;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
            waveFormat.cbSize = 0;

            // Primary Buffer
            DSBUFFERDESC dsBufferDescription = {};
            dsBufferDescription.dwSize = sizeof(dsBufferDescription);
            dsBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            LPDIRECTSOUNDBUFFER dsPrimaryBuffer;
            if (SUCCEEDED(directSound->CreateSoundBuffer(&dsBufferDescription, &dsPrimaryBuffer, nullptr))) {
                if (SUCCEEDED(dsPrimaryBuffer->SetFormat(&waveFormat))) {
                    OutputDebugStringA("Primary audio buffer created.\n");
                } else {
                    // TODO: log
                }
            }

            // Secondary Buffer
            DSBUFFERDESC dsSecBufferDescription = {};
            dsSecBufferDescription.dwSize = sizeof(dsSecBufferDescription);
            dsSecBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
            dsSecBufferDescription.dwBufferBytes = GlobalAudioBuffer.bufferSizeBytes;
            dsSecBufferDescription.lpwfxFormat = &waveFormat;
            if (SUCCEEDED(
                directSound->CreateSoundBuffer(&dsSecBufferDescription, &GlobalAudioBuffer.buffer, nullptr)
            )) { OutputDebugStringA("Secondary audio buffer created.\n"); } else {
                //TODO: log
            }
        }
    }
}

static DWORD winLoadAudio(DWORD lockCursor, DWORD targetCursor) {
    DWORD numBytesToWrite = 0;
    if (lockCursor >
        targetCursor) { numBytesToWrite = GlobalAudioBuffer.bufferSizeBytes - lockCursor + targetCursor; } else {
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
        OutputDebugStringA("Audio buffer lock failed.\n");
        return 0;
    }

    audio::AudioBuffer region1Buffer = {};
    region1Buffer.samples = static_cast<audio::audio_sample_t*>(audioRegion1Ptr);
    region1Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region1Buffer.numSamplesToWrite = audioRegion1Bytes / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.loadAudio(region1Buffer);

    audio::AudioBuffer region2Buffer = {};
    region2Buffer.samples = static_cast<audio::audio_sample_t*>(audioRegion2Ptr);
    region2Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region2Buffer.numSamplesToWrite = audioRegion2Bytes / GlobalAudioBuffer.bytesPerSample;
    // TODO: maybe just make 1 call to loadAudio with both buffers or have some ring buffer representation?
    GlobalSlurpDll.loadAudio(region2Buffer);

    GlobalAudioBuffer.buffer->Unlock(
        audioRegion1Ptr,
        audioRegion1Bytes,
        audioRegion2Ptr,
        audioRegion2Bytes
    );
    return numBytesToWrite;
}

static bool winInitialize(HINSTANCE instance, HWND* outWindowHandle) {
    winLoadXInput();

    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW;
    windowClass.lpfnWndProc = winMessageHandler;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = WINDOW_CLASS_NAME;
    RegisterClassA(&windowClass);

    winResizeDIBSection(&GlobalGraphicsBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    *outWindowHandle = CreateWindowExA(
        WS_EX_TOPMOST,
        windowClass.lpszClassName,
        "Slurp's Up!",
        WS_MAXIMIZE | WS_OVERLAPPEDWINDOW | WS_VISIBLE | CS_OWNDC,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!*outWindowHandle) {
        OutputDebugStringA("Failed to create window.\n");
        return false;
    }

    GlobalRunning = true;

    return true;
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
        OutputDebugStringA("Could not fetch monitor refresh rate.\n");
        return DEFAULT_MONITOR_REFRESH_RATE;
    }
    return devMode.dmDisplayFrequency;
}

static void winAllocateGameMemory(platform::GameMemory* outGameMemory) {
    uint64_t permanentMemorySizeBytes = megabytes(64);
    uint64_t transientMemorySizeBytes = gigabytes(4);
    outGameMemory->permanentMemory.sizeBytes = permanentMemorySizeBytes;
    outGameMemory->transientMemory.sizeBytes = transientMemorySizeBytes;
#if DEBUG
    void* baseAddress = (void*) terabytes(1);
#else
	void* baseAddress = nullptr;
#endif
    void* memory = VirtualAlloc(
        baseAddress,
        permanentMemorySizeBytes + transientMemorySizeBytes,
        MEM_RESERVE | MEM_COMMIT,
        // TODO: could we use MEM_LARGE_PAGES to alleviate TLB
        PAGE_READWRITE
    );
    outGameMemory->permanentMemory.memory = memory;
    outGameMemory->transientMemory.memory = static_cast<uint8_t*>(memory) + permanentMemorySizeBytes;
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
        // OutputDebugStringA("Frame too slow. Target frame rate missed.\n");
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

#if VERBOSE
    std::cout << std::format("Frame: {:.2f}ms {}fps {} processor mega-cycles", frameMillis, fps, frameProcessorMCycles) << std::endl;
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
        char buf[256];
        sprintf_s(buf, "Failed to load lib function: %s.\n", fnName);
        OutputDebugStringA(buf);
        assert(out);
        out = stubFn;
    }
}

static void winLoadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath) {
    CopyFileA(dllFilePath, dllLoadFilePath, false);
    GlobalSlurpLib = LoadLibraryA(SLURP_LOAD_DLL_FILE_NAME);
    if (!GlobalSlurpLib) { OutputDebugStringA("Failed to load SlurpEngine.dll.\n"); } else {
        winLoadLibFn<slurp::dyn_init>(
            GlobalSlurpDll.init,
            "init",
            slurp::stub_init,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_handleInput>(
            GlobalSlurpDll.handleInput,
            "handleInput",
            slurp::stub_handleInput,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_loadAudio>(
            GlobalSlurpDll.loadAudio,
            "loadAudio",
            slurp::stub_loadAudio,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_updateAndRender>(
            GlobalSlurpDll.updateAndRender,
            "updateAndRender",
            slurp::stub_updateAndRender,
            GlobalSlurpLib
        );
    }
}

static void winUnloadSlurpLib() {
    if (GlobalSlurpLib && !FreeLibrary(GlobalSlurpLib)) { OutputDebugStringA("Failed to unload Slurp lib.\n"); }
    GlobalSlurpDll = slurp::SlurpDll();
}

static void winTryReloadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath) {
    HANDLE dllFileHandle = CreateFileA(
        dllFilePath,
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
    if (!GetFileTime(dllFileHandle, &_, &_, &writeTime)) {
        OutputDebugStringA("Failed to get SlurpEngine.dll file time.\n");
    }
    CloseHandle(dllFileHandle);
    if (CompareFileTime(&writeTime, &previousWriteTime) == 0) { return; }

    previousWriteTime = writeTime;
    winUnloadSlurpLib();
    winLoadSlurpLib(dllFilePath, dllLoadFilePath);
    GlobalSlurpDll.init(GlobalPlatformDll, GlobalGameMemory);
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
        OutputDebugStringA("Invalid file handle.");
        return result;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(
        fileHandle,
        &fileSize
    );

    assert(fileSize.QuadPart < gigabytes(4));
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
        OutputDebugStringA("Could not read file.");
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
        OutputDebugStringA("Invalid file handle.");
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
        OutputDebugStringA("Could not write file.");
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
    WriteFile(
        GlobalRecordingState.recordingFileHandle,
        GlobalGameMemory.permanentMemory.memory,
        static_cast<DWORD>(GlobalGameMemory.permanentMemory.sizeBytes),
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
    ReadFile(
        GlobalRecordingState.recordingFileHandle,
        GlobalGameMemory.permanentMemory.memory,
        static_cast<DWORD>(GlobalGameMemory.permanentMemory.sizeBytes),
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

void winDrawDebugLine(int drawX, uint32_t color) {
    int lineWidth = 8;

    byte* bitmapBytes = static_cast<byte*>(GlobalGraphicsBuffer.memory) + drawX * GlobalGraphicsBuffer.
                        bytesPerPixel;
    for (int y = 0; y < GlobalGraphicsBuffer.heightPixels; y++) {
        uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
        for (int x = 0; x < lineWidth; x++) {
            if (drawX + x >= GlobalGraphicsBuffer.widthPixels) { return; }
            *rowPixels++ = color;
        }

        bitmapBytes += GlobalGraphicsBuffer.pitchBytes;
    }
}

void winDrawDebugAudioSync(DWORD cursor, uint32_t color) {
    int x = static_cast<int>(
        (static_cast<float>(cursor) / GlobalAudioBuffer.bufferSizeBytes) *
        (GlobalGraphicsBuffer.widthPixels)
    );
    winDrawDebugLine(x, color);
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

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow
) {
    HWND windowHandle;
    if (!winInitialize(hInstance, &windowHandle)) { return 1; }

    std::string dllFilePathStr = getLocalFilePath(SLURP_DLL_FILE_NAME);
    const char* dllFilePath = dllFilePathStr.c_str();
    std::string dllLoadFilePathStr = getLocalFilePath(SLURP_LOAD_DLL_FILE_NAME);
    const char* dllLoadFilePath = dllLoadFilePathStr.c_str();
    winLoadSlurpLib(dllFilePath, dllLoadFilePath);

    GlobalPlatformDll = loadPlatformDll();
    winAllocateGameMemory(&GlobalGameMemory);
    winTryReloadSlurpLib(dllFilePath, dllLoadFilePath);

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
        winTryReloadSlurpLib(dllFilePath, dllLoadFilePath);

        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry: keyboardState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        for (std::pair<const slurp::MouseCode, slurp::DigitalInputState>& entry: mouseState.state) {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        WinScreenDimensions dimensions = winGetScreenDimensions(windowHandle);
        winHandleMessages(keyboardState, mouseState, dimensions, GlobalGraphicsBuffer);
        winHandleGamepadInput(gamepadStates);
#if DEBUG
        if (GlobalRecordingState.isRecording) { winRecordInput(mouseState, keyboardState, gamepadStates); }
        if (GlobalRecordingState.isPlayingBack) { winReadInputRecording(mouseState, keyboardState, gamepadStates); }
#endif
        GlobalSlurpDll.handleInput(mouseState, keyboardState, gamepadStates);

#if DEBUG
        if (GlobalRecordingState.isPaused) { continue; }
#endif

        render::GraphicsBuffer graphicsBuffer = {
            static_cast<render::Pixel*>(GlobalGraphicsBuffer.memory),
            GlobalGraphicsBuffer.widthPixels,
            GlobalGraphicsBuffer.heightPixels
        };
        GlobalSlurpDll.updateAndRender(graphicsBuffer, targetSecondsPerFrame);

        if (GlobalAudioBuffer.buffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK) {
            OutputDebugStringA("Get audio buffer position failed.\n");
        }
        // NOTE: We always set our targetCursor to right after the write cursor,
        // this means audio is played as soon as possible.
        // TODO: if the sound card has very low latency, we could play audio earlier than the frame flip
        DWORD targetCursor = (
                                 writeCursor +
                                 GlobalAudioBuffer.writeAheadSampleCount * GlobalAudioBuffer.bytesPerSample
                             ) % GlobalAudioBuffer.bufferSizeBytes;
        DWORD numBytesWritten = winLoadAudio(lockCursor, targetCursor);
        lockCursor = (lockCursor + numBytesWritten) % GlobalAudioBuffer.bufferSizeBytes;

        winStallFrameToTarget(targetMillisPerFrame, startTimingInfo, isSleepGranular);
        winCaptureAndLogPerformance(startProcessorCycle, startTimingInfo);

#if 0
        winDrawDebugAudioSync(playCursor, 0x00000000);
        winDrawDebugAudioSync(lockCursor, 0x00FF0000);
#endif
        HDC deviceContext = GetDC(windowHandle);
        winUpdateWindow(deviceContext, GlobalGraphicsBuffer, dimensions.width, dimensions.height);
        ReleaseDC(windowHandle, deviceContext);
    }

    if (isSleepGranular) { timeEndPeriod(1); }
    return 0;
}
