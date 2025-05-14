#include <iostream>
#include <ostream>
#include <Platform.hpp>
#include <WinEngine.hpp>

#include <shlwapi.h>
#include <string>

#define kilobytes(n) ((int64_t)n * 1024)
#define megabytes(n) (kilobytes(n) * 1024)
#define gigabytes(n) (megabytes(n) * 1024)
#define terabytes(n) (gigabytes(n) * 1024)

#if DEBUG
#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 720
#else
#define DISPLAY_WIDTH 2560
#define DISPLAY_HEIGHT 1440
#endif
#define DEFAULT_MONITOR_REFRESH_RATE 144

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

static WinScreenDimensions winGetScreenDimensions(HWND windowHandle)
{
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

static void winResizeDIBSection(WinGraphicsBuffer* outBuffer, int width, int height)
{
    if (outBuffer->memory)
    {
        VirtualFree(outBuffer->memory, 0, MEM_RELEASE);
    }

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
    const WinGraphicsBuffer buffer,
    int screenWidth,
    int screenHeight
)
{
    // TODO: aspect ratio correction
    StretchDIBits(
        deviceContextHandle,
#if 1
        0, 0, buffer.widthPixels, buffer.heightPixels, // for pixel perfect scaling
#else
        0, 0, screenWidth, screenHeight,
#endif
        0, 0, buffer.widthPixels, buffer.heightPixels,
        buffer.memory,
        &buffer.info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
};

static void winPaint(HWND windowHandle, const WinGraphicsBuffer buffer)
{
    PAINTSTRUCT paintStruct;
    HDC deviceContext = BeginPaint(windowHandle, &paintStruct);
    WinScreenDimensions dimensions = winGetScreenDimensions(windowHandle);
    winUpdateWindow(deviceContext, buffer, dimensions.width, dimensions.height);
    EndPaint(windowHandle, &paintStruct);
    ReleaseDC(windowHandle, deviceContext);
}


static LRESULT CALLBACK winMessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
    case WM_ACTIVATEAPP:
        {
        }
        break;
    case WM_SIZE:
        {
        }
        break;
    case WM_DESTROY:
    case WM_CLOSE:
        {
            GlobalRunning = false;
        }
        break;
    case WM_PAINT:
        {
            winPaint(windowHandle, GlobalGraphicsBuffer);
        }
        break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            assert(!"Keyboard event should not be handled in Windows handler.");
        }
        break;
    default:
        {
            result = DefWindowProcA(windowHandle, message, wParam, lParam);
        }
        break;
    }

    return result;
};

static void winHandleMouseInput(HWND windowHandle, slurp::MouseState* outMouseState)
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(windowHandle, &point);
    outMouseState->position = {point.x, point.y};

    short keyDownBit = static_cast<short>(1 << 15);
    for (std::pair<WinMouseCode, slurp::MouseCode> entry : MouseWinCodeToSlurpCode)
    {
        WinMouseCode winMouseCode = entry.first;
        bool isDown = GetKeyState(winMouseCode) & keyDownBit;

        slurp::MouseCode mouseCode = entry.second;
        slurp::DigitalInputState* inputState = &outMouseState->state[mouseCode];
        inputState->transitionCount = inputState->isDown != isDown ? 1 : 0;
        inputState->isDown = isDown;
    }
}

static void winHandleMessages(slurp::KeyboardState* keyboardState)
{
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            {
                WPARAM virtualKeyCode = message.wParam;
                bool wasDown = (1 << 30) & message.lParam;
                bool isDown = ((1 << 31) & message.lParam) == 0;

                if (KeyboardWinCodeToSlurpCode.count(virtualKeyCode) > 0)
                {
                    slurp::KeyboardCode code = KeyboardWinCodeToSlurpCode.at(virtualKeyCode);
                    // TODO: change transition count computation once we poll multiple times per frame
                    slurp::DigitalInputState* inputState = &keyboardState->state[code];
                    inputState->transitionCount = wasDown != isDown ? 1 : 0;
                    // TODO: do we need to clear this every frame?
                    inputState->isDown = isDown;
                }
                else
                {
                    OutputDebugStringA("Windows keyboard code not registered.\n");
                }
            }
            break;
        case WM_QUIT:
            {
                GlobalRunning = false;
            }
            break;
        default:
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
}

#define X_INPUT_GET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

static x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}

static x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

static void winLoadXInput()
{
    HMODULE xInputLib = LoadLibraryA("xinput1_4.dll");
    if (!xInputLib)
    {
        // TODO: log
        xInputLib = LoadLibraryA("xinput1_3.dll");
    }
    if (xInputLib)
    {
        XInputGetState = reinterpret_cast<x_input_get_state*>(GetProcAddress(xInputLib, "XInputGetState"));
        XInputSetState = reinterpret_cast<x_input_set_state*>(GetProcAddress(xInputLib, "XInputSetState"));
    }
    else
    {
        // TODO: log
    }
}

#define XINPUT_STICK_MAG_POS 32767
#define XINPUT_STICK_MAG_NEG 32768
#define XINPUT_TRIGGER_MAG 255
#define XINPUT_VIBRATION_MAG 65535

static float winGetNormalizedStickValue(int16_t stickValue, int16_t deadZone)
{
    if (abs(stickValue) < deadZone)
    {
        return 0.f;
    }
    if (stickValue > 0)
    {
        return static_cast<float>(stickValue) / XINPUT_STICK_MAG_POS;
    }
    return static_cast<float>(stickValue) / XINPUT_STICK_MAG_NEG;
}

static float winGetNormalizedTriggerValue(uint8_t triggerValue)
{
    if (triggerValue < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
    {
        return 0.f;
    }
    return static_cast<float>(triggerValue) / XINPUT_TRIGGER_MAG;
}

static void winHandleGamepadInput(slurp::GamepadState* controllerStates)
{
    for (DWORD controllerIdx = 0; controllerIdx < XUSER_MAX_COUNT; controllerIdx++)
    {
        XINPUT_STATE xInputState;
        DWORD result = XInputGetState(controllerIdx, &xInputState);
        if (result == ERROR_SUCCESS)
        {
            slurp::GamepadState* gamepadState = &controllerStates[controllerIdx];
            gamepadState->isConnected = true;

            XINPUT_GAMEPAD gamepad = xInputState.Gamepad;
            for (std::pair<XInputCode, slurp::GamepadCode> entry : GamepadWinCodeToSlurpCode)
            {
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
        }
        else
        {
            // Controller is not connected
            // TODO: log
        }
    }
};

#define DIRECT_SOUND_CREATE(fnName) HRESULT WINAPI fnName(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

static void winInitDirectSound(HWND windowHandle)
{
    HMODULE dSoundLib = LoadLibraryA("dsound.dll");
    if (dSoundLib)
    {
        direct_sound_create* directSoundCreate = reinterpret_cast<direct_sound_create*>(GetProcAddress(
            dSoundLib, "DirectSoundCreate"));
        LPDIRECTSOUND directSound;
        if (directSoundCreate && SUCCEEDED(directSoundCreate(nullptr, &directSound, nullptr)))
        {
            directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY);

            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 2;
            waveFormat.nSamplesPerSec = GlobalAudioBuffer.samplesPerSec;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
            waveFormat.cbSize = 0;

            // Primary Buffer
            DSBUFFERDESC dsBufferDescription = {};
            dsBufferDescription.dwSize = sizeof(dsBufferDescription);
            dsBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            LPDIRECTSOUNDBUFFER dsPrimaryBuffer;
            if (SUCCEEDED(directSound->CreateSoundBuffer(&dsBufferDescription, &dsPrimaryBuffer, nullptr)))
            {
                if (SUCCEEDED(dsPrimaryBuffer->SetFormat(&waveFormat)))
                {
                    OutputDebugStringA("Primary audio buffer created.\n");
                }
                else
                {
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
                directSound->CreateSoundBuffer(&dsSecBufferDescription, &GlobalAudioBuffer.buffer, nullptr)))
            {
                OutputDebugStringA("Secondary audio buffer created.\n");
            }
            else
            {
                //TODO: log
            }
        }
    }
}

static DWORD winLoadAudio(DWORD lockCursor, DWORD targetCursor)
{
    DWORD numBytesToWrite = 0;
    if (lockCursor > targetCursor)
    {
        numBytesToWrite = GlobalAudioBuffer.bufferSizeBytes - lockCursor + targetCursor;
    }
    else
    {
        numBytesToWrite = targetCursor - lockCursor;
    }

    if (numBytesToWrite == 0)
    {
        return 0;
    }

    void* audioRegion1Ptr;
    DWORD audioRegion1Bytes;
    void* audioRegion2Ptr;
    DWORD audioRegion2Bytes;
    if (!SUCCEEDED(GlobalAudioBuffer.buffer->Lock(
        lockCursor,
        numBytesToWrite,
        &audioRegion1Ptr,
        &audioRegion1Bytes,
        &audioRegion2Ptr,
        &audioRegion2Bytes,
        0
    )))
    {
        OutputDebugStringA("Audio buffer lock failed.\n");
        return 0;
    }

    slurp::AudioBuffer region1Buffer = {};
    region1Buffer.samples = static_cast<int32_t*>(audioRegion1Ptr);
    region1Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region1Buffer.samplesToWrite = audioRegion1Bytes / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.loadAudio(region1Buffer);

    slurp::AudioBuffer region2Buffer = {};
    region2Buffer.samples = static_cast<int32_t*>(audioRegion2Ptr);
    region2Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region2Buffer.samplesToWrite = audioRegion2Bytes / GlobalAudioBuffer.bytesPerSample;
    GlobalSlurpDll.loadAudio(region2Buffer);

    GlobalAudioBuffer.buffer->Unlock(
        audioRegion1Ptr,
        audioRegion1Bytes,
        audioRegion2Ptr,
        audioRegion2Bytes
    );
    return numBytesToWrite;
}

static bool winInitialize(HINSTANCE instance, HWND* outWindowHandle)
{
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
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | CS_OWNDC,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!*outWindowHandle)
    {
        OutputDebugStringA("Failed to create window.\n");
        return false;
    }

    GlobalRunning = true;

    return true;
}


static DWORD winGetMonitorRefreshRate()
{
    DEVMODEA devMode = {};
    devMode.dmSize = sizeof(devMode);
    if (!EnumDisplaySettingsExA(
        nullptr,
        ENUM_CURRENT_SETTINGS,
        &devMode,
        EDS_RAWMODE
    ))
    {
        OutputDebugStringA("Could not fetch monitor refresh rate.\n");
        return DEFAULT_MONITOR_REFRESH_RATE;
    }
    return devMode.dmDisplayFrequency;
}

static void winAllocateGameMemory(platform::GameMemory* outGameMemory)
{
    uint64_t permanentMemorySizeBytes = megabytes(64);
    uint64_t transientMemorySizeBytes = gigabytes(4);
    outGameMemory->permanentMemory.sizeBytes = permanentMemorySizeBytes;
    outGameMemory->transientMemory.sizeBytes = transientMemorySizeBytes;
#if DEBUG
    void* baseAddress = (void*)terabytes(1);
#else
	void* baseAddress = nullptr;
#endif
    void* memory = VirtualAlloc(
        baseAddress,
        permanentMemorySizeBytes + transientMemorySizeBytes,
        MEM_RESERVE | MEM_COMMIT, // TODO: could we use MEM_LARGE_PAGES to alleviate TLB
        PAGE_READWRITE
    );
    outGameMemory->permanentMemory.memory = memory;
    outGameMemory->transientMemory.memory = static_cast<uint8_t*>(memory) + permanentMemorySizeBytes;
}

static float winGetFrameMillis(
    WinTimingInfo startTimingInfo,
    LARGE_INTEGER& outPerformanceCounterEnd
)
{
    QueryPerformanceCounter(&outPerformanceCounterEnd);
    return (outPerformanceCounterEnd.QuadPart - startTimingInfo.performanceCounter) * 1000.f /
        startTimingInfo.performanceCounterFrequency;
}

#define SLEEP_STALL_MIN_LEFTOVER_MS 0.5f

static void winStallFrameToTarget(
    float targetMillisPerFrame,
    WinTimingInfo startTimingInfo,
    bool isSleepGranular
)
{
    LARGE_INTEGER performanceCounterEnd;
    float frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    if (frameMillis >= targetMillisPerFrame)
    {
        // OutputDebugStringA("Frame too slow. Target frame rate missed.\n");
        return;
    }
    if (isSleepGranular)
    {
        float stallMs = targetMillisPerFrame - frameMillis;
        DWORD sleepMs = static_cast<DWORD>(stallMs);
        if (sleepMs > 1 && (stallMs - sleepMs) > SLEEP_STALL_MIN_LEFTOVER_MS)
        {
            sleepMs -= 1;
        }
        if (sleepMs > 0)
        {
            Sleep(sleepMs);
        }
    }
    while (frameMillis < targetMillisPerFrame)
    {
        frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    }
}

static void winCaptureAndLogPerformance(
    uint64_t& startProcessorCycle,
    WinTimingInfo& startTimingInfo
)
{
    uint64_t processorCycleEnd = __rdtsc();
    LARGE_INTEGER performanceCounterEnd;

    float frameMillis = winGetFrameMillis(startTimingInfo, performanceCounterEnd);
    int fps = static_cast<int>(1000 / frameMillis);
    int frameProcessorMCycles = static_cast<int>((processorCycleEnd - startProcessorCycle) / 1000 / 1000);

    // char buf[256];
    // sprintf_s(buf, "Frame: %.2fms %dfps %d processor mega-cycles\n", frameMillis, fps, frameProcessorMCycles);
    // OutputDebugStringA(buf);

    startProcessorCycle = processorCycleEnd;
    startTimingInfo.performanceCounter = performanceCounterEnd.QuadPart;
}

template <typename T>
static void winLoadLibFn(T*& out, LPCSTR fnName, T* stubFn, const HMODULE& lib)
{
    out = reinterpret_cast<T*>(
        GetProcAddress(lib, fnName)
    );
    if (!out)
    {
        char buf[256];
        sprintf_s(buf, "Failed to load lib function: %s.\n", fnName);
        OutputDebugStringA(buf);
        assert(out);
        out = stubFn;
    }
}

static void winLoadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath)
{
    CopyFileA(dllFilePath, dllLoadFilePath, false);
    GlobalSlurpLib = LoadLibraryA(SLURP_LOAD_DLL_FILE_NAME);
    if (!GlobalSlurpLib)
    {
        OutputDebugStringA("Failed to load SlurpEngine.dll.\n");
    }
    else
    {
        winLoadLibFn<slurp::dyn_init>(
            GlobalSlurpDll.init,
            "init",
            slurp::stub_init,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_handleMouseAndKeyboardInput>(
            GlobalSlurpDll.handleMouseAndKeyboardInput,
            "handleMouseAndKeyboardInput",
            slurp::stub_handleMouseAndKeyboardInput,
            GlobalSlurpLib
        );
        winLoadLibFn<slurp::dyn_handleGamepadInput>(
            GlobalSlurpDll.handleGamepadInput,
            "handleGamepadInput",
            slurp::stub_handleGamepadInput,
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

static void winUnloadSlurpLib()
{
    if (GlobalSlurpLib && !FreeLibrary(GlobalSlurpLib))
    {
        OutputDebugStringA("Failed to unload Slurp lib.\n");
    }
    GlobalSlurpDll = slurp::SlurpDll();
}

static void winTryReloadSlurpLib(const char* dllFilePath, const char* dllLoadFilePath)
{
    HANDLE dllFileHandle = CreateFileA(
        dllFilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    static FILETIME previousWriteTime;
    FILETIME writeTime;
    FILETIME _;
    if (!GetFileTime(dllFileHandle, &_, &_, &writeTime))
    {
        OutputDebugStringA("Failed to get SlurpEngine.dll file time.\n");
    }
    CloseHandle(dllFileHandle);
    if (CompareFileTime(&writeTime, &previousWriteTime) == 0)
    {
        return;
    }

    previousWriteTime = writeTime;
    winUnloadSlurpLib();
    winLoadSlurpLib(dllFilePath, dllLoadFilePath);
    GlobalSlurpDll.init(GlobalPlatformDll, &GlobalGameMemory);
}

static std::string getLocalFilePath(LPCSTR filename)
{
    char exeDirPath[MAX_PATH];
    GetModuleFileNameA(nullptr, exeDirPath, MAX_PATH);
    PathRemoveFileSpecA(exeDirPath);
    std::string exeDirPathStr = std::string(exeDirPath);
    return exeDirPathStr + "\\" + filename;
}

#if DEBUG
PLATFORM_DEBUG_READ_FILE(platform::DEBUG_readFile)
{
    platform::DEBUG_FileReadResult result = {};
    HANDLE fileHandle = CreateFileA(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
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

    if (!success || bytesRead != fileSizeTruncated)
    {
        OutputDebugStringA("Could not read file.");
        platform::DEBUG_freeMemory(buffer);
        return result;
    }

    result.fileContents = buffer;
    result.sizeBytes = fileSizeTruncated;
    return result;
}

PLATFORM_DEBUG_WRITE_FILE(platform::DEBUG_writeFile)
{
    HANDLE fileHandle = CreateFileA(
        fileName,
        GENERIC_WRITE,
        FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
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

    if (!success || bytesWritten != sizeBytes)
    {
        OutputDebugStringA("Could not write file.");
        return false;
    }
    return true;
}

PLATFORM_DEBUG_FREE_MEMORY(platform::DEBUG_freeMemory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

PLATFORM_DEBUG_TOGGLE_PAUSE(platform::DEBUG_togglePause)
{
    GlobalRecordingState.isPaused = !GlobalRecordingState.isPaused;
}

PLATFORM_DEBUG_BEGIN_RECORDING(platform::DEBUG_beginRecording)
{
    GlobalRecordingState.recordingFileHandle = CreateFileA(
        getLocalFilePath(RECORDING_FILE_NAME).c_str(),
        GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
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

static void winRecordInput(const slurp::KeyboardState& keyboardState,
                           slurp::GamepadState gamepadStates[MAX_NUM_CONTROLLERS])
{
    DWORD _;
    // TODO: record mouse input

    size_t numKeyboardStates = keyboardState.state.size();
    WriteFile(
        GlobalRecordingState.recordingFileHandle,
        &numKeyboardStates,
        sizeof(size_t),
        &_,
        nullptr
    );
    for (const slurp::keyboard_state_entry& entry : keyboardState.state)
    {
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &entry,
            sizeof(slurp::keyboard_state_entry),
            &_,
            nullptr
        );
    }

    for (int controllerIdx = 0; controllerIdx < MAX_NUM_CONTROLLERS; controllerIdx++)
    {
        slurp::GamepadState& gamepadState = gamepadStates[controllerIdx];
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.isConnected,
            sizeof(bool),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.leftStick,
            sizeof(slurp::AnalogStickInputState),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.rightStick,
            sizeof(slurp::AnalogStickInputState),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.leftTrigger,
            sizeof(slurp::AnalogTriggerInputState),
            &_,
            nullptr
        );
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &gamepadState.rightTrigger,
            sizeof(slurp::AnalogTriggerInputState),
            &_,
            nullptr
        );
        size_t numGamepadStates = gamepadState.state.size();
        WriteFile(
            GlobalRecordingState.recordingFileHandle,
            &numGamepadStates,
            sizeof(size_t),
            &_,
            nullptr
        );
        for (const slurp::gamepad_state_entry& entry : gamepadState.state)
        {
            WriteFile(
                GlobalRecordingState.recordingFileHandle,
                &entry,
                sizeof(slurp::gamepad_state_entry),
                &_,
                nullptr
            );
        }
    }
}

PLATFORM_DEBUG_END_RECORDING(platform::DEBUG_endRecording)
{
    GlobalRecordingState.isRecording = false;
    CloseHandle(GlobalRecordingState.recordingFileHandle);
}

PLATFORM_DEBUG_BEGIN_PLAYBACK(platform::DEBUG_beginPlayback)
{
    GlobalRecordingState.recordingFileHandle = CreateFileA(
        getLocalFilePath(RECORDING_FILE_NAME).c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
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

static void winReadInputRecording(
    slurp::KeyboardState& outKeyboardState,
    slurp::GamepadState outGamepadStates[MAX_NUM_CONTROLLERS]
)
{
    DWORD bytesRead;
    // TODO: read mouse input

    size_t numKeyboardStates = 0;
    ReadFile(
        GlobalRecordingState.recordingFileHandle,
        &numKeyboardStates,
        sizeof(size_t),
        &bytesRead,
        nullptr
    );
    if (bytesRead == 0)
    {
        GlobalRecordingState.isPlayingBack = false;
        CloseHandle(GlobalRecordingState.recordingFileHandle);
        GlobalRecordingState.onPlaybackEnd();
    }
    outKeyboardState.state.clear();
    for (int i = 0; i < numKeyboardStates; i++)
    {
        slurp::keyboard_state_entry entry;
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &entry,
            sizeof(slurp::keyboard_state_entry),
            &bytesRead,
            nullptr
        );
        outKeyboardState.state.insert(entry);
    }

    for (int controllerIdx = 0; controllerIdx < MAX_NUM_CONTROLLERS; controllerIdx++)
    {
        slurp::GamepadState& outGamepadState = outGamepadStates[controllerIdx];
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.isConnected,
            sizeof(bool),
            &bytesRead,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.leftStick,
            sizeof(slurp::AnalogStickInputState),
            &bytesRead,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.rightStick,
            sizeof(slurp::AnalogStickInputState),
            &bytesRead,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.leftTrigger,
            sizeof(slurp::AnalogTriggerInputState),
            &bytesRead,
            nullptr
        );
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &outGamepadState.rightTrigger,
            sizeof(slurp::AnalogTriggerInputState),
            &bytesRead,
            nullptr
        );
        size_t numGamepadStates = 0;
        ReadFile(
            GlobalRecordingState.recordingFileHandle,
            &numGamepadStates,
            sizeof(size_t),
            &bytesRead,
            nullptr
        );
        outGamepadState.state.clear();
        for (int i = 0; i < numGamepadStates; i++)
        {
            slurp::gamepad_state_entry entry;
            ReadFile(
                GlobalRecordingState.recordingFileHandle,
                &entry,
                sizeof(slurp::gamepad_state_entry),
                &bytesRead,
                nullptr
            );
            outGamepadState.state.insert(entry);
        }
    }
}

void winDrawDebugLine(int drawX, uint32_t color)
{
    int lineWidth = 8;

    byte* bitmapBytes = static_cast<byte*>(GlobalGraphicsBuffer.memory) + drawX * GlobalGraphicsBuffer.bytesPerPixel;
    for (int y = 0; y < GlobalGraphicsBuffer.heightPixels; y++)
    {
        uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
        for (int x = 0; x < lineWidth; x++)
        {
            if (drawX + x >= GlobalGraphicsBuffer.widthPixels)
            {
                return;
            }
            *rowPixels++ = color;
        }

        bitmapBytes += GlobalGraphicsBuffer.pitchBytes;
    }
}

void winDrawDebugAudioSync(DWORD cursor, uint32_t color)
{
    int padX = 16;
    int x = static_cast<int>((static_cast<float>(cursor) / GlobalAudioBuffer.bufferSizeBytes) * (
        GlobalGraphicsBuffer.widthPixels));
    winDrawDebugLine(x, color);
}
#endif

PLATFORM_VIBRATE_CONTROLLER(platform::vibrateController)
{
    uint16_t leftMotorSpeedRaw = static_cast<uint16_t>(leftMotorSpeed * XINPUT_VIBRATION_MAG);
    uint16_t rightMotorSpeedRaw = static_cast<uint16_t>(rightMotorSpeed * XINPUT_VIBRATION_MAG);
    XINPUT_VIBRATION vibration{
        leftMotorSpeedRaw,
        rightMotorSpeedRaw,
    };
    XInputSetState(controllerIdx, &vibration);
}

PLATFORM_SHUTDOWN(platform::shutdown)
{
    GlobalRunning = false;
}

static platform::PlatformDll loadPlatformDll()
{
    platform::PlatformDll platformDll = {};
    platformDll.vibrateController = platform::vibrateController;
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
)
{
    HWND windowHandle;
    if (!winInitialize(hInstance, &windowHandle))
    {
        return 1;
    }

    std::string dllFilePathStr = getLocalFilePath(SLURP_DLL_FILE_NAME);
    const char* dllFilePath = dllFilePathStr.c_str();
    std::string dllLoadFilePathStr = getLocalFilePath(SLURP_LOAD_DLL_FILE_NAME);
    const char* dllLoadFilePath = dllLoadFilePathStr.c_str();
    winLoadSlurpLib(dllFilePath, dllLoadFilePath);

    GlobalPlatformDll = loadPlatformDll();
    winAllocateGameMemory(&GlobalGameMemory);
    GlobalSlurpDll.init(GlobalPlatformDll, &GlobalGameMemory);

    bool isSleepGranular = timeBeginPeriod(1) == TIMERR_NOERROR;
    DWORD targetFramesPerSecond = winGetMonitorRefreshRate();
    float targetSecondsPerFrame = 1.f / targetFramesPerSecond;
    float targetMillisPerFrame = targetSecondsPerFrame * 1000.f;

    slurp::MouseState mouseState;
    slurp::KeyboardState keyboardState;
    slurp::GamepadState controllerStates[MAX_NUM_CONTROLLERS];

    // TODO: migrate to the newer XAudio2
    // TODO: could dynamically track max writeCursor diff and update GlobalAudioBuffer.writeAheadSampleCount
    winInitDirectSound(windowHandle);
    DWORD playCursor = 0;
    DWORD writeCursor = 0;
    DWORD lockCursor = 0;
    GlobalAudioBuffer.buffer->Play(NULL, NULL, DSBPLAY_LOOPING);

    uint64_t startProcessorCycle = __rdtsc();
    LARGE_INTEGER startPerformanceCounter;
    QueryPerformanceCounter(&startPerformanceCounter);
    LARGE_INTEGER performanceCounterFrequency;
    QueryPerformanceFrequency(&performanceCounterFrequency);
    WinTimingInfo startTimingInfo = {
        startPerformanceCounter.QuadPart,
        performanceCounterFrequency.QuadPart
    };

    while (GlobalRunning)
    {
        winTryReloadSlurpLib(dllFilePath, dllLoadFilePath);

        winHandleMouseInput(windowHandle, &mouseState);
        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry : keyboardState.state)
        {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        winHandleMessages(&keyboardState);
        winHandleGamepadInput(controllerStates);
#if DEBUG
        if (GlobalRecordingState.isRecording)
        {
            winRecordInput(keyboardState, controllerStates);
        }
        if (GlobalRecordingState.isPlayingBack)
        {
            winReadInputRecording(keyboardState, controllerStates);
        }
#endif
        GlobalSlurpDll.handleMouseAndKeyboardInput(mouseState, keyboardState, targetSecondsPerFrame);
        GlobalSlurpDll.handleGamepadInput(controllerStates, targetSecondsPerFrame);

#if DEBUG
        if (GlobalRecordingState.isPaused)
        {
            continue;
        }
#endif

        slurp::GraphicsBuffer graphicsBuffer = {
            static_cast<slurp::Pixel* const>(GlobalGraphicsBuffer.memory),
            GlobalGraphicsBuffer.widthPixels,
            GlobalGraphicsBuffer.heightPixels
        };
        GlobalSlurpDll.updateAndRender(graphicsBuffer, targetSecondsPerFrame);

        if (GlobalAudioBuffer.buffer->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
        {
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

        WinScreenDimensions dimensions = winGetScreenDimensions(windowHandle);
#if 0
        winDrawDebugAudioSync(playCursor, 0x00000000);
        winDrawDebugAudioSync(lockCursor, 0x00FF0000);
#endif
        HDC deviceContext = GetDC(windowHandle);
        winUpdateWindow(deviceContext, GlobalGraphicsBuffer, dimensions.width, dimensions.height);
        ReleaseDC(windowHandle, deviceContext);
    }

    if (isSleepGranular)
    {
        timeEndPeriod(1);
    }
    return 0;
}
