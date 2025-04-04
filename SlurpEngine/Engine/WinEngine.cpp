#include <iostream>
#include <windows.h>

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

static bool Running;

struct WinGraphicsBuffer
{
    BITMAPINFO Info;
    void* Memory;
    int WidthPixels;
    int HeightPixels;
    int BytesPerPixel;
};

static WinGraphicsBuffer GlobalBackBuffer;

static void RenderCoolGraphics(const WinGraphicsBuffer Buffer, int XOffset, int YOffset)
{
    int RandBound = 20;

    int PitchBytes = Buffer.WidthPixels * Buffer.BytesPerPixel;
    byte* BitmapBytes = static_cast<byte*>(Buffer.Memory);
    for (int Y = 0; Y < Buffer.HeightPixels; Y++)
    {
        uint32_t* RowPixels = reinterpret_cast<uint32_t*>(BitmapBytes);
        for (int X = 0; X < Buffer.WidthPixels; X++)
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

static void WinResizeDIBSection(WinGraphicsBuffer* OutBuffer, int Width, int Height)
{
    if (OutBuffer->Memory)
    {
        VirtualFree(OutBuffer->Memory, 0, MEM_RELEASE);
    }

    OutBuffer->WidthPixels = Width;
    OutBuffer->HeightPixels = Height;
    OutBuffer->BytesPerPixel = 4;

    OutBuffer->Info.bmiHeader.biSize = sizeof(OutBuffer->Info.bmiHeader);
    OutBuffer->Info.bmiHeader.biWidth = OutBuffer->WidthPixels;
    OutBuffer->Info.bmiHeader.biHeight = -OutBuffer->HeightPixels;
    OutBuffer->Info.bmiHeader.biPlanes = 1;
    OutBuffer->Info.bmiHeader.biBitCount = OutBuffer->BytesPerPixel * 8;
    OutBuffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapSizeBytes = OutBuffer->WidthPixels * OutBuffer->HeightPixels * OutBuffer->BytesPerPixel;
    OutBuffer->Memory = VirtualAlloc(nullptr, BitmapSizeBytes, MEM_COMMIT, PAGE_READWRITE);
}

static void WinUpdateWindow(HDC DeviceContextHandle, RECT WindowRect, const WinGraphicsBuffer Buffer)
{
    int WindowX = WindowRect.left;
    int WindowY = WindowRect.top;
    int WindowWidth = WindowRect.right - WindowRect.left;
    int WindowHeight = WindowRect.bottom - WindowRect.top;
    StretchDIBits(
        DeviceContextHandle,
        WindowX, WindowY, WindowWidth, WindowHeight,
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
    RECT Rect;

    HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);
    GetClientRect(WindowHandle, &Rect);
    EndPaint(WindowHandle, &PaintStruct);
    WinUpdateWindow(DeviceContext, Rect, Buffer);
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
            WinResizeDIBSection(&GlobalBackBuffer, Width, Height);
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
            WinPaint(WindowHandle, GlobalBackBuffer);
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
    WindowClass.style = CS_OWNDC | CS_HREDRAW;
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
            RenderCoolGraphics(GlobalBackBuffer, dX++ + (rand() % 10), dY++ + (rand() % 10));
            WinPaint(WindowHandle, GlobalBackBuffer);
        }
    }

    return 0;
}
