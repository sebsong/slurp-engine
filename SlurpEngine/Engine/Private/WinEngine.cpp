#include <SlurpEngine.cpp>

#include <iostream>
#include <windows.h>
#include <Xinput.h>
#include <dsound.h>

typedef int32_t bool32;
#define PI 3.14159265359f

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

struct WinGraphicsBuffer
{
    BITMAPINFO info;
    void* memory;
    int widthPixels;
    int heightPixels;
    int pitchBytes;
};

struct WinScreenDimensions
{
    int x;
    int y;
    int width;
    int height;
};

struct WinAudioBuffer
{
    LPDIRECTSOUNDBUFFER buffer;
    int samplesPerSec = 48000;
    int bytesPerSample = sizeof(int16_t) * 2; // Stereo L + R
    int bufferSizeBytes = samplesPerSec * bytesPerSample;
    int writeAheadSampleCount = samplesPerSec / 20;
    float frequencyHz = 300;
};

static bool GlobalRunning;
static WinGraphicsBuffer GlobalBackBuffer;
static WinAudioBuffer GlobalAudioBuffer;
static float dX = 0;
static float dY = 0;

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
    outBuffer->pitchBytes = width * bytesPerPixel;

    outBuffer->info.bmiHeader.biSize = sizeof(outBuffer->info.bmiHeader);
    outBuffer->info.bmiHeader.biWidth = outBuffer->widthPixels;
    outBuffer->info.bmiHeader.biHeight = -outBuffer->heightPixels;
    outBuffer->info.bmiHeader.biPlanes = 1;
    outBuffer->info.bmiHeader.biBitCount = bytesPerPixel * 8;
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


LRESULT CALLBACK winMessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
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
            winPaint(windowHandle, GlobalBackBuffer);
        }
        break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            WPARAM virtualKeyCode = wParam;
            bool32 wasDown = (1 << 30) & lParam;
            bool32 isDown = (1 << 31) & lParam;

            bool32 alt = (1 << 29) & lParam;
            if (alt && isDown && virtualKeyCode == VK_F4)
            {
                GlobalRunning = false;
            }

            static float scrollSpeed = 255;
            static float ddX = 0;
            static float ddY = 0;

            dX += scrollSpeed * ddX / 50;
            dY += scrollSpeed * ddY / 50;

            if (wasDown == isDown)
            {
                break;
            }

            switch (virtualKeyCode)
            {
            case 'W':
                {
                    if (isDown)
                    {
                        ddY -= 1;
                    }
                    else
                    {
                        ddY += 1;
                    }
                }
                break;
            case 'A':
                {
                    if (isDown)
                    {
                        ddX -= 1;
                    }
                    else
                    {
                        ddX += 1;
                    }
                }
                break;
            case 'S':
                {
                    if (isDown)
                    {
                        ddY += 1;
                    }
                    else
                    {
                        ddY -= 1;
                    }
                }
                break;
            case 'D':
                {
                    if (isDown)
                    {
                        ddX += 1;
                    }
                    else
                    {
                        ddX -= 1;
                    }
                }
                break;
            case VK_ESCAPE:
                {
                    GlobalRunning = false;
                }
                break;
            case VK_SPACE:
                {
                    if (isDown)
                    {
                        scrollSpeed *= 5;
                    }
                    else
                    {
                        scrollSpeed /= 5;
                    }
                }
                break;
            default:
                {
                }
                break;
            }
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

void winDrainMessages()
{
    MSG message;
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            GlobalRunning = false;
        }
        TranslateMessage(&message);
        DispatchMessageA(&message);
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

void winHandleGamepadInput()
{
    for (DWORD controllerIdx = 0; controllerIdx < XUSER_MAX_COUNT; controllerIdx++)
    {
        XINPUT_STATE state;
        DWORD result = XInputGetState(controllerIdx, &state);
        if (result == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD gamepad = state.Gamepad;
            bool dPadUp = gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
            bool dPadDown = gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
            bool dPadLeft = gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
            bool dPadRight = gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
            bool start = gamepad.wButtons & XINPUT_GAMEPAD_START;
            bool back = gamepad.wButtons & XINPUT_GAMEPAD_BACK;
            bool leftThumb = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
            bool rightThumb = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
            bool leftShoulder = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
            bool rightShoulder = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            bool aButton = gamepad.wButtons & XINPUT_GAMEPAD_A;
            bool bButton = gamepad.wButtons & XINPUT_GAMEPAD_B;
            bool xButton = gamepad.wButtons & XINPUT_GAMEPAD_X;
            bool yButton = gamepad.wButtons & XINPUT_GAMEPAD_Y;

            uint8_t leftTrigger = gamepad.bLeftTrigger;
            uint8_t rightTrigger = gamepad.bRightTrigger;
            int16_t leftStickX = gamepad.sThumbLX;
            int16_t leftStickY = gamepad.sThumbLY;
            int16_t rightStickX = gamepad.sThumbRX;
            int16_t rightStickY = gamepad.sThumbRY;

            if (bButton || start)
            {
                GlobalRunning = false;
            }

            int scrollSpeed = 1;
            if (leftShoulder || rightShoulder)
            {
                scrollSpeed *= 5;
            }
            dX += (float)(leftStickX * scrollSpeed) / 20000;
            dY -= (float)(leftStickY * scrollSpeed) / 20000;

            uint16_t leftMotorSpeed = (uint32_t)(leftTrigger * 65535) / 255;
            uint16_t rightMotorSpeed = (uint32_t)(rightTrigger * 65535) / 255;
            XINPUT_VIBRATION vibration{
                leftMotorSpeed,
                rightMotorSpeed
            };
            XInputSetState(controllerIdx, &vibration);

            GlobalAudioBuffer.frequencyHz = 360 + (leftStickX / 65535.f) * 220;
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
                    OutputDebugStringA("PRIMARY CREATED");
                }
                else
                {
                    // TODO: log
                }
            }

            // Secondary Buffer
            DSBUFFERDESC dsSecBufferDescription = {};
            dsSecBufferDescription.dwSize = sizeof(dsSecBufferDescription);
            dsSecBufferDescription.dwFlags = 0;
            dsSecBufferDescription.dwBufferBytes = GlobalAudioBuffer.bufferSizeBytes;
            dsSecBufferDescription.lpwfxFormat = &waveFormat;
            if (SUCCEEDED(
                directSound->CreateSoundBuffer(&dsSecBufferDescription, &GlobalAudioBuffer.buffer, nullptr)))
            {
                OutputDebugStringA("SECONDARY CREATED");
            }
            else
            {
                //TODO: log
            }
        }
    }
}

static void winLoadAudio(bool isInitialLoad)
{
    static DWORD writeCursor = 0;
    DWORD playCursor;
    DWORD _;
    if (!SUCCEEDED(GlobalAudioBuffer.buffer->GetCurrentPosition(&playCursor,&_)))
    {
        OutputDebugStringA("Get audio buffer position failed.\n");
        return;
    }
    DWORD targetCursor = (
        playCursor +
        GlobalAudioBuffer.writeAheadSampleCount * GlobalAudioBuffer.bytesPerSample
    ) % GlobalAudioBuffer.bufferSizeBytes;
    
    DWORD numBytesToWrite = 0;
    if (writeCursor == targetCursor)
    {
        if (isInitialLoad)
        {
            numBytesToWrite = GlobalAudioBuffer.writeAheadSampleCount;
        }
    }
    else if (writeCursor > targetCursor)
    {
        numBytesToWrite = GlobalAudioBuffer.bufferSizeBytes - writeCursor + targetCursor;
    }
    else
    {
        numBytesToWrite = targetCursor - writeCursor;
    }

    if (numBytesToWrite == 0)
    {
        return;
    }

    void* audioRegion1Ptr;
    DWORD audioRegion1Bytes;
    void* audioRegion2Ptr;
    DWORD audioRegion2Bytes;
    if (!SUCCEEDED(GlobalAudioBuffer.buffer->Lock(
        writeCursor,
        numBytesToWrite,
        &audioRegion1Ptr,
        &audioRegion1Bytes,
        &audioRegion2Ptr,
        &audioRegion2Bytes,
        0
    )))
    {
        OutputDebugStringA("Audio buffer lock failed.\n");
        return;
    }

    slurp::AudioBuffer region1Buffer = {};
    region1Buffer.samples = static_cast<int32_t*>(audioRegion1Ptr);
    region1Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region1Buffer.samplesToWrite = audioRegion1Bytes / GlobalAudioBuffer.bytesPerSample;
    region1Buffer.frequencyHz = GlobalAudioBuffer.frequencyHz;
    slurp::loadAudio(region1Buffer);

    slurp::AudioBuffer region2Buffer = {};
    region2Buffer.samples = static_cast<int32_t*>(audioRegion2Ptr);
    region2Buffer.samplesPerSec = GlobalAudioBuffer.samplesPerSec;
    region2Buffer.samplesToWrite = audioRegion2Bytes / GlobalAudioBuffer.bytesPerSample;
    region2Buffer.frequencyHz = GlobalAudioBuffer.frequencyHz;
    slurp::loadAudio(region2Buffer);

    writeCursor = (writeCursor + numBytesToWrite) % GlobalAudioBuffer.bufferSizeBytes;

    GlobalAudioBuffer.buffer->Unlock(
        audioRegion1Ptr,
        audioRegion1Bytes,
        audioRegion2Ptr,
        audioRegion2Bytes
    );
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

    winResizeDIBSection(&GlobalBackBuffer, 1280, 720);

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

void winCaptureAndLogPerformance(
    uint64_t& previousProcessorCycle,
    LARGE_INTEGER& previousPerformanceCounter,
    LARGE_INTEGER performanceCounterFrequency
)
{
    uint64_t processorCycleEnd = __rdtsc();
    LARGE_INTEGER performanceCounterEnd;
    QueryPerformanceCounter(&performanceCounterEnd);

    float frameMillis = (performanceCounterEnd.QuadPart - previousPerformanceCounter.QuadPart) * 1000.f /
        performanceCounterFrequency.QuadPart;
    int fps = 1000 / frameMillis;
    int frameProcessorMCycles = (processorCycleEnd - previousProcessorCycle) / 1000 / 1000;

    char buf[256];
    sprintf_s(buf, "Frame: %.2fms %dfps %d processor mega-cycles\n", frameMillis, fps, frameProcessorMCycles);
    OutputDebugStringA(buf);

    previousProcessorCycle = processorCycleEnd;
    previousPerformanceCounter = performanceCounterEnd;
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

    winInitDirectSound(windowHandle);
    bool isPlayingAudio = false;

    uint64_t processorCycle = __rdtsc();
    LARGE_INTEGER performanceCounterFrequency;
    QueryPerformanceFrequency(&performanceCounterFrequency);
    LARGE_INTEGER performanceCounter;
    QueryPerformanceCounter(&performanceCounter);

    HDC deviceContext = GetDC(windowHandle);
    while (GlobalRunning)
    {
        winDrainMessages();
        winHandleGamepadInput();

        slurp::GraphicsBuffer graphicsBuffer = {};
        graphicsBuffer.memory = GlobalBackBuffer.memory;
        graphicsBuffer.widthPixels = GlobalBackBuffer.widthPixels;
        graphicsBuffer.heightPixels = GlobalBackBuffer.heightPixels;
        graphicsBuffer.pitchBytes = GlobalBackBuffer.pitchBytes;
        graphicsBuffer.dX = dX;
        graphicsBuffer.dY = dY;
        slurp::renderGraphics(graphicsBuffer);

        winLoadAudio(!isPlayingAudio);
        if (!isPlayingAudio)
        {
            GlobalAudioBuffer.buffer->Play(NULL, NULL, DSBPLAY_LOOPING);
            isPlayingAudio = true;
        }

        WinScreenDimensions dimensions = winGetScreenDimensions(windowHandle);
        winUpdateWindow(deviceContext, GlobalBackBuffer, dimensions.width, dimensions.height);
        ReleaseDC(windowHandle, deviceContext);

        winCaptureAndLogPerformance(
            processorCycle,
            performanceCounter,
            performanceCounterFrequency
        );
    }

    return 0;
}
