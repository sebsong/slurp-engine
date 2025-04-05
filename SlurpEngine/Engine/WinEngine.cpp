#include <iostream>
#include <windows.h>
#include <Xinput.h>

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

static bool Running;
static float dX = 0;
static float dY = 0;

struct WinGraphicsBuffer
{
    BITMAPINFO Info;
    void* Memory;
    int WidthPixels;
    int HeightPixels;
    int PitchBytes;
};

static WinGraphicsBuffer GlobalBackBuffer;

struct WinScreenDimensions
{
    int X;
    int Y;
    int Width;
    int Height;
};

static WinScreenDimensions WinGetScreenDimensions(HWND WindowHandle)
{
    RECT Rect;
    GetClientRect(WindowHandle, &Rect);
    return WinScreenDimensions
    {
        Rect.left,
        Rect.top,
        Rect.right - Rect.left,
        Rect.bottom - Rect.top,
    };
};

static void RenderCoolGraphics(const WinGraphicsBuffer Buffer, float XOffset, float YOffset)
{
    byte* BitmapBytes = static_cast<byte*>(Buffer.Memory);
    for (int Y = 0; Y < Buffer.HeightPixels; Y++)
    {
        uint32_t* RowPixels = reinterpret_cast<uint32_t*>(BitmapBytes);
        for (int X = 0; X < Buffer.WidthPixels; X++)
        {
            uint8_t R = Y + YOffset;
            uint8_t G = (X + XOffset) - (Y + YOffset);
            uint8_t B = X + XOffset;

            uint32_t Pixel = (R << 16) | (G << 8) | B;
            *RowPixels++ = Pixel;
        }

        BitmapBytes += Buffer.PitchBytes;
    }
}

static void WinResizeDIBSection(WinGraphicsBuffer* OutBuffer, int Width, int Height)
{
    if (OutBuffer->Memory)
    {
        VirtualFree(OutBuffer->Memory, 0, MEM_RELEASE);
    }

    int BytesPerPixel = 4;
    OutBuffer->WidthPixels = Width;
    OutBuffer->HeightPixels = Height;
    OutBuffer->PitchBytes = Width * BytesPerPixel;

    OutBuffer->Info.bmiHeader.biSize = sizeof(OutBuffer->Info.bmiHeader);
    OutBuffer->Info.bmiHeader.biWidth = OutBuffer->WidthPixels;
    OutBuffer->Info.bmiHeader.biHeight = -OutBuffer->HeightPixels;
    OutBuffer->Info.bmiHeader.biPlanes = 1;
    OutBuffer->Info.bmiHeader.biBitCount = BytesPerPixel * 8;
    OutBuffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapSizeBytes = OutBuffer->WidthPixels * OutBuffer->HeightPixels * BytesPerPixel;
    OutBuffer->Memory = VirtualAlloc(nullptr, BitmapSizeBytes, MEM_COMMIT, PAGE_READWRITE);
}

static void WinUpdateWindow(
    HDC DeviceContextHandle,
    const WinGraphicsBuffer Buffer,
    int ScreenWidth,
    int ScreenHeight
)
{
    // TODO: aspect ratio correction
    StretchDIBits(
        DeviceContextHandle,
        0, 0, ScreenWidth, ScreenHeight,
        0, 0, Buffer.WidthPixels, Buffer.HeightPixels,
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
};

static void WinPaint(HWND WindowHandle, const WinGraphicsBuffer Buffer)
{
    PAINTSTRUCT PaintStruct;
    HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);
    WinScreenDimensions Dimensions = WinGetScreenDimensions(WindowHandle);
    WinUpdateWindow(DeviceContext, Buffer, Dimensions.Width, Dimensions.Height);
    EndPaint(WindowHandle, &PaintStruct);
    ReleaseDC(WindowHandle, DeviceContext);
}


LRESULT CALLBACK WinMessageHandler(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;

    switch (Message)
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
            Running = false;
        }
        break;
    case WM_PAINT:
        {
            WinPaint(WindowHandle, GlobalBackBuffer);
        }
        break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            WPARAM VirtualKeyCode = wParam;
            bool WasDown = ((1 << 30) & lParam) != 0;
            bool IsDown = ((1 << 31) & lParam) == 0;

            if (WasDown == IsDown)
            {
                break;
            }

            bool WKey = VirtualKeyCode == 'W';
            bool AKey = VirtualKeyCode == 'A';
            bool SKey = VirtualKeyCode == 'S';
            bool DKey = VirtualKeyCode == 'D';
            bool Escape = VirtualKeyCode == VK_ESCAPE;
            bool Space = VirtualKeyCode == VK_SPACE;

            int ScrollSpeed = 255;

            switch (VirtualKeyCode)
            {
            case 'W':
                {
                }
                break;
            case 'A':
                {
                }
                break;
            case 'S':
                {
                }
                break;
            case 'D':
                {
                }
                break;
            case VK_ESCAPE:
                {
                    Running = false;
                }
                break;
            case VK_SPACE:
                {
                    ScrollSpeed *= 5;
                    if (IsDown)
                    {
                        OutputDebugStringA("SPACE DOWN\n");
                    }
                    else
                    {
                        OutputDebugStringA("SPACE UP\n");
                    }
                }
                break;
            }

            dX += (float)(WKey * ScrollSpeed) / 20000;
            dX -= (float)(SKey * ScrollSpeed) / 20000;
            dY -= (float)(AKey * ScrollSpeed) / 20000;
            dY += (float)(DKey * ScrollSpeed) / 20000;
        }
        break;
    default:
        {
            Result = DefWindowProcA(WindowHandle, Message, wParam, lParam);
        }
        break;
    }

    return Result;
};

void WinDrainMessages()
{
    MSG Message;
    while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        if (Message.message == WM_QUIT)
        {
            Running = false;
        }
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }
}

#define X_INPUT_GET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return 0;
}

static x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(fnName) DWORD WINAPI fnName(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return 0;
}

static x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

static void WinLoadXInput()
{
    HMODULE XInputLib = LoadLibraryA("xinput1_3.dll");
    if (XInputLib)
    {
        XInputGetState = reinterpret_cast<x_input_get_state*>(GetProcAddress(XInputLib, "XInputGetState"));
        XInputSetState = reinterpret_cast<x_input_set_state*>(GetProcAddress(XInputLib, "XInputSetState"));
    }
}

void HandleGamepadInput()
{
    for (DWORD ControllerIdx = 0; ControllerIdx < XUSER_MAX_COUNT; ControllerIdx++)
    {
        XINPUT_STATE State;
        DWORD Result = XInputGetState(ControllerIdx, &State);
        if (Result == ERROR_SUCCESS)
        {
            XINPUT_GAMEPAD Gamepad = State.Gamepad;
            bool DPadUp = Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
            bool DPadDown = Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
            bool DPadLeft = Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
            bool DPadRight = Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
            bool Start = Gamepad.wButtons & XINPUT_GAMEPAD_START;
            bool Back = Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
            bool LeftThumb = Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
            bool RightThumb = Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
            bool LeftShoulder = Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
            bool RightShoulder = Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            bool AButton = Gamepad.wButtons & XINPUT_GAMEPAD_A;
            bool BButton = Gamepad.wButtons & XINPUT_GAMEPAD_B;
            bool XButton = Gamepad.wButtons & XINPUT_GAMEPAD_X;
            bool YButton = Gamepad.wButtons & XINPUT_GAMEPAD_Y;

            uint8_t LeftTrigger = Gamepad.bLeftTrigger;
            uint8_t RightTrigger = Gamepad.bRightTrigger;
            int16_t LeftStickX = Gamepad.sThumbLX;
            int16_t LeftStickY = Gamepad.sThumbLY;
            int16_t RightStickX = Gamepad.sThumbRX;
            int16_t RightStickY = Gamepad.sThumbRY;

            if (BButton || Start)
            {
                Running = false;
            }

            int ScrollSpeed = 1;
            if (LeftShoulder || RightShoulder)
            {
                ScrollSpeed *= 5;
            }
            dX += (float)(LeftStickX * ScrollSpeed) / 20000;
            dY -= (float)(LeftStickY * ScrollSpeed) / 20000;

            uint16_t LeftMotorSpeed = (uint32_t)(LeftTrigger * 65535) / 255;
            uint16_t RightMotorSpeed = (uint32_t)(RightTrigger * 65535) / 255;
            XINPUT_VIBRATION Vibration{
                LeftMotorSpeed,
                RightMotorSpeed
            };
            XInputSetState(ControllerIdx, &Vibration);
        }
        else
        {
            // Controller is not connected
        }
    }
};

static bool WinInitialize(HINSTANCE Instance, HWND* OutWindowHandle)
{
    WinLoadXInput();

    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_OWNDC | CS_HREDRAW;
    WindowClass.lpfnWndProc = WinMessageHandler;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = WINDOW_CLASS_NAME;
    RegisterClassA(&WindowClass);

    WinResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    *OutWindowHandle = CreateWindowExA(
        0,
        WindowClass.lpszClassName,
        "Slurp's Up!",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | CS_OWNDC,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        Instance,
        nullptr
    );

    if (!*OutWindowHandle)
    {
        OutputDebugStringA("FAILED");
        return false;
    }

    Running = true;

    return true;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow
)
{
    HWND WindowHandle;
    WinInitialize(hInstance, &WindowHandle);

    HDC DeviceContext = GetDC(WindowHandle);
    while (Running)
    {
        WinDrainMessages();

        HandleGamepadInput();

        RenderCoolGraphics(GlobalBackBuffer, dX, dY);
        WinScreenDimensions Dimensions = WinGetScreenDimensions(WindowHandle);
        WinUpdateWindow(DeviceContext, GlobalBackBuffer, Dimensions.Width, Dimensions.Height);
        ReleaseDC(WindowHandle, DeviceContext);
    }

    return 0;
}
