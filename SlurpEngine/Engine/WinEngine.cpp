#include <iostream>
#include <windows.h>

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";
static const int BYTES_PER_PIXEL = 4;

static bool Running;
static BITMAPINFO BitmapInfo;
static void* BitmapMemory;
static int BitmapWidthPixels;
static int BitmapHeightPixels;

static void RenderCoolGraphics(int XOffset, int YOffset)
{
    int RandBound = 20;

    int PitchBytes = BitmapWidthPixels * BYTES_PER_PIXEL;
    byte* BitmapBytes = static_cast<byte*>(BitmapMemory);
    for (int Y = 0; Y < BitmapHeightPixels; Y++)
    {
        uint32_t* RowPixels = reinterpret_cast<uint32_t*>(BitmapBytes);
        for (int X = 0; X < BitmapWidthPixels; X++)
        {
            uint8_t R = Y + YOffset + rand() % RandBound;
            uint8_t G = (X + XOffset) - (Y + YOffset) + rand() % RandBound;
            uint8_t B = X + XOffset + rand() % RandBound;

            uint32_t Pixel = (R << 16) | (G << 8) | B;
            *RowPixels++ = Pixel;
        }

        BitmapBytes += PitchBytes;
    }
}

static void WinResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidthPixels = Width;
    BitmapHeightPixels = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidthPixels;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeightPixels;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = BYTES_PER_PIXEL * 8;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapSizeBytes = BitmapWidthPixels * BitmapHeightPixels * BYTES_PER_PIXEL;
    BitmapMemory = VirtualAlloc(nullptr, BitmapSizeBytes, MEM_COMMIT, PAGE_READWRITE);

    RenderCoolGraphics(128, 128);
}

static void WinUpdateWindow(HDC DeviceContextHandle, RECT* WindowRect)
{
    int WindowX = WindowRect->left;
    int WindowY = WindowRect->top;
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;
    StretchDIBits(
        DeviceContextHandle,
        WindowX, WindowY, WindowWidth, WindowHeight,
        0, 0, BitmapWidthPixels, BitmapHeightPixels,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
};

static void Paint(HWND WindowHandle)
{
    PAINTSTRUCT PaintStruct;
    RECT Rect;

    HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);
    GetClientRect(WindowHandle, &Rect);
    EndPaint(WindowHandle, &PaintStruct);
    WinUpdateWindow(DeviceContext, &Rect);
    ReleaseDC(WindowHandle, DeviceContext);
}


LRESULT CALLBACK WindowProc(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
    case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("ACTIVATE\n");
        }
        break;
    case WM_SIZE:
        {
            RECT Rect;
            GetClientRect(WindowHandle, &Rect);
            int Width = Rect.right - Rect.left;
            int Height = Rect.bottom - Rect.top;
            WinResizeDIBSection(Width, Height);
            OutputDebugStringA("SIZE\n");
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
            Paint(WindowHandle);
            OutputDebugStringA("PAINT\n");
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

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR lpCmdLine,
    int nCmdShow
)
{
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = WINDOW_CLASS_NAME;

    RegisterClassA(&WindowClass);

    HWND WindowHandle = CreateWindowExA(
        0,
        WindowClass.lpszClassName,
        "Slurp's Up!",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!WindowHandle)
    {
        OutputDebugStringA("FAILED");
        return 0;
    }

    Running = true;

    MSG Message;
    while (Running)
    {
        if (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
        {
            if (Message.message == WM_QUIT)
            {
                Running = false;
            }
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        }
        else
        {
            static int dX = 0;
            static int dY = 0;
            RenderCoolGraphics(dX++ + (rand() % 10), dY++ + (rand() % 10));
            Paint(WindowHandle);
        }
    }

    return 0;
}
