#include <iostream>
#include <windows.h>

static const LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

static bool Running;
static BITMAPINFO BitmapInfo;
static void* BitmapMemory;
static HBITMAP BitmapHandle;
static HDC DeviceContextHandle;

static void WinResizeDIBSection(int Width, int Height)
{
    if (BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }

    if (!DeviceContextHandle)
    {
        DeviceContextHandle = CreateCompatibleDC(nullptr);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    BitmapHandle = CreateDIBSection(
        DeviceContextHandle,
        &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        NULL,
        NULL
    );
}

static void WinUpdateWindow(HDC DeviceContextHandle, int X, int Y, int Width, int Height)
{
    StretchDIBits(
        DeviceContextHandle,
        X, Y, Width, Height,
        X, Y, Width, Height,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
};

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
            PAINTSTRUCT PaintStruct;
            HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);
            RECT PaintRect = PaintStruct.rcPaint;
            int X = PaintRect.left;
            int Y = PaintRect.top;
            int Width = PaintRect.right - PaintRect.left;
            int Height = PaintRect.bottom - PaintRect.top;
            PatBlt(
                DeviceContext,
                X,
                Y,
                Width,
                Height,
                BLACKNESS
            );
            EndPaint(WindowHandle, &PaintStruct);
            WinUpdateWindow(DeviceContext, X, Y, Width, Height);
            OutputDebugStringA("PAINT\n");
        }
        break;
    default:
        {
            // OutputDebugStringA("DEFAULT\n");
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
    while (Running && GetMessageA(&Message, nullptr, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }

    return 0;
}
