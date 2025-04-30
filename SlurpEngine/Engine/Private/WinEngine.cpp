#include <Platform.hpp>
#include <WinEngine.hpp>

#define kilobytes(n) ((int64_t)n * 1024)
#define megabytes(n) (kilobytes(n) * 1024)
#define gigabytes(n) (megabytes(n) * 1024)
#define terabytes(n) (gigabytes(n) * 1024)

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

static bool GlobalRunning;
static bool GlobalPause;
static WinGraphicsBuffer GlobalGraphicsBuffer;
static WinAudioBuffer GlobalAudioBuffer;

static slurp::SlurpDLL GlobalSlurpDLL;
static HMODULE GlobalSlurpLib;

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
        0, 0, screenWidth, screenHeight,
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
            assert(!"Keyboard event should not be handled in Windows handler.")
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

                // bool32 alt = (1 << 29) & lParam;
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
            leftStickState.startXY = leftStickState.endXY;
            leftStickState.endXY.x = leftStickXNormalized;
            leftStickState.endXY.y = leftStickYNormalized;

            float rightStickXNormalized = winGetNormalizedStickValue(
                gamepad.sThumbRX,
                XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
            );
            float rightStickYNormalized = winGetNormalizedStickValue(
                gamepad.sThumbRY,
                XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
            );
            slurp::AnalogStickInputState& rightStickState = gamepadState->rightStick;
            rightStickState.startXY = rightStickState.endXY;
            rightStickState.endXY.x = rightStickXNormalized;
            rightStickState.endXY.y = rightStickYNormalized;

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
    // slurp::loadAudio(region1Buffer);
    GlobalSlurpDLL.loadAudio(region1Buffer);

    slurp::AudioBuffer region2Buffer = {};
    region2Buffer.samples = static_cast<int32_t*>(audioRegion2Ptr);
    region2Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region2Buffer.samplesToWrite = audioRegion2Bytes / GlobalAudioBuffer.bytesPerSample;
    // slurp::loadAudio(region2Buffer);
    GlobalSlurpDLL.loadAudio(region2Buffer);

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
    windowClass.style = CS_OWNDC | CS_HREDRAW;
    windowClass.lpfnWndProc = winMessageHandler;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = WINDOW_CLASS_NAME;
    RegisterClassA(&windowClass);

    winResizeDIBSection(&GlobalGraphicsBuffer, 1280, 720);

    *outWindowHandle = CreateWindowExA(
        0,
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

#define DEFAULT_MONITOR_REFRESH_RATE 60

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

static void winAllocateGameMemory(slurp::GameMemory* outGameMemory)
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
        MEM_RESERVE | MEM_COMMIT,
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

static slurp::SlurpDLL winLoadSlurpEngineLib()
{
    slurp::SlurpDLL slurpDLL;
    GlobalSlurpLib = LoadLibraryA("SlurpEngine.dll");
    if (!GlobalSlurpLib)
    {
        OutputDebugStringA("Failed to load SlurpEngine.dll.\n");
    }
    else
    {
        slurpDLL.init = reinterpret_cast<slurp::slurp_init*>(
            GetProcAddress(GlobalSlurpLib, "init")
        );
        if (!slurpDLL.init)
        {
            OutputDebugStringA("Failed to load slurp::init.\n");
            slurpDLL.init = slurp::slurp_init_stub;
        }
        
        slurpDLL.handleKeyboardInput = reinterpret_cast<slurp::slurp_handle_keyboard_input*>(
            GetProcAddress(GlobalSlurpLib, "handleKeyboardInput")
        );
        if (!slurpDLL.handleKeyboardInput)
        {
            OutputDebugStringA("Failed to load slurp::handleKeyboardInput.\n");
            slurpDLL.handleKeyboardInput = slurp::slurp_handle_keyboard_input_stub;
        }
        
        slurpDLL.handleGamepadInput = reinterpret_cast<slurp::slurp_handle_gamepad_input*>(
            GetProcAddress(GlobalSlurpLib, "handleGamepadInput")
        );
        if (!slurpDLL.handleGamepadInput)
        {
            OutputDebugStringA("Failed to load slurp::handleGamepadInput.\n");
            slurpDLL.handleGamepadInput = slurp::slurp_handle_gamepad_input_stub;
        }
        
        slurpDLL.loadAudio = reinterpret_cast<slurp::slurp_load_audio*>(
            GetProcAddress(GlobalSlurpLib, "loadAudio")
        );
        if (!slurpDLL.loadAudio)
        {
            OutputDebugStringA("Failed to load slurp::loadAudio.\n");
            slurpDLL.loadAudio = slurp::slurp_load_audio_stub;
        }
        
        slurpDLL.renderGraphics = reinterpret_cast<slurp::slurp_render_graphics*>(
            GetProcAddress(GlobalSlurpLib, "renderGraphics")
        );
        if (!slurpDLL.renderGraphics)
        {
            OutputDebugStringA("Failed to load slurp::renderGraphics.\n");
            slurpDLL.renderGraphics = slurp::slurp_render_graphics_stub;
        }
    }
    
    return slurpDLL;
}

#if DEBUG
DEBUG_FileReadResult DEBUG_platformReadFile(const char* fileName)
{
    DEBUG_FileReadResult result = {};
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

    assert(fileSize.QuadPart < gigabytes(4))
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
        DEBUG_platformFreeMemory(buffer);
        return result;
    }

    result.fileContents = buffer;
    result.sizeBytes = fileSizeTruncated;
    return result;
}

bool DEBUG_platformWriteFile(const char* fileName, void* fileContents, uint32_t sizeBytes)
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

void DEBUG_platformFreeMemory(void* memory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

void DEBUG_platformTogglePause()
{
    GlobalPause = !GlobalPause;
}
#endif

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

void platformVibrateController(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed)
{
    uint16_t leftMotorSpeedRaw = static_cast<uint16_t>(leftMotorSpeed * XINPUT_VIBRATION_MAG);
    uint16_t rightMotorSpeedRaw = static_cast<uint16_t>(rightMotorSpeed * XINPUT_VIBRATION_MAG);
    XINPUT_VIBRATION vibration{
        leftMotorSpeedRaw,
        rightMotorSpeedRaw,
    };
    XInputSetState(controllerIdx, &vibration);
}

void platformShutdown()
{
    GlobalRunning = false;
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

    GlobalSlurpDLL = winLoadSlurpEngineLib();

    PlatformDLL platformDLL = {};
    platformDLL.platformVibrateController = platformVibrateController;
    platformDLL.platformShutdown = platformShutdown;
    platformDLL.DEBUG_platformTogglePause = DEBUG_platformTogglePause;
    slurp::GameMemory gameMemory = {};
    winAllocateGameMemory(&gameMemory);
    // slurp::init(&gameMemory);
    GlobalSlurpDLL.init(platformDLL, &gameMemory);

    bool isSleepGranular = timeBeginPeriod(1) == TIMERR_NOERROR;
    DWORD targetFramesPerSecond = winGetMonitorRefreshRate();
    float targetMillisPerFrame = 1000.f / targetFramesPerSecond;

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

    HDC deviceContext = GetDC(windowHandle);

    while (GlobalRunning)
    {
        for (std::pair<const slurp::KeyboardCode, slurp::DigitalInputState>& entry : keyboardState.state)
        {
            slurp::DigitalInputState& inputState = entry.second;
            inputState.transitionCount = 0;
        }
        winHandleMessages(&keyboardState);
        // slurp::handleKeyboardInput(keyboardState);
        GlobalSlurpDLL.handleKeyboardInput(keyboardState);
        winHandleGamepadInput(controllerStates);
        // slurp::handleGamepadInput(controllerStates);
        GlobalSlurpDLL.handleGamepadInput(controllerStates);
        
#if DEBUG
        if (GlobalPause)
        {
            continue;
        }
#endif

        slurp::GraphicsBuffer graphicsBuffer = {};
        graphicsBuffer.memory = GlobalGraphicsBuffer.memory;
        graphicsBuffer.widthPixels = GlobalGraphicsBuffer.widthPixels;
        graphicsBuffer.heightPixels = GlobalGraphicsBuffer.heightPixels;
        graphicsBuffer.pitchBytes = GlobalGraphicsBuffer.pitchBytes;
        // slurp::renderGraphics(graphicsBuffer);
        GlobalSlurpDLL.renderGraphics(graphicsBuffer);

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
#if DEBUG
        winDrawDebugAudioSync(playCursor, 0x00000000);
        winDrawDebugAudioSync(lockCursor, 0x00FF0000);
#endif
        winUpdateWindow(deviceContext, GlobalGraphicsBuffer, dimensions.width, dimensions.height);
        ReleaseDC(windowHandle, deviceContext);
    }

    if (isSleepGranular)
    {
        timeEndPeriod(1);
    }
    return 0;
}
