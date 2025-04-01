#include <iostream>
#include <ostream>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    MessageBox(0, "Slurp's Up", "Slurp Engine", MB_OK|MB_ICONINFORMATION);
    return 0;
}