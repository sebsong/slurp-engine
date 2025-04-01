#include <iostream>
#include <windows.h>

LPCSTR WINDOW_CLASS_NAME = "SlurpEngineWindowClass";

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
            OutputDebugStringA("SIZE\n");
        }
        break;
    case WM_DESTROY:
        {
            OutputDebugStringA("DESTROY\n");
        }
        break;
    case WM_CLOSE:
        {
            OutputDebugStringA("CLOSE\n");
            Result = DefWindowProcA(WindowHandle, Message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            HDC DeviceContext = BeginPaint(WindowHandle, &PaintStruct);
            RECT PaintRect = PaintStruct.rcPaint;
            PatBlt(
                DeviceContext,
                PaintRect.left,
                PaintRect.top,
                PaintRect.right - PaintRect.left,
                PaintRect.bottom - PaintRect.top,
                BLACKNESS
            );
            EndPaint(WindowHandle, &PaintStruct);
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

    MSG Message;
    while (GetMessageA(&Message, nullptr, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }

    return 0;
}
