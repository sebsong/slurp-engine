#pragma once
#include <cstdint>

#define DLL_EXPORT
#if PLATFORM_WINDOWS
#undef DLL_EXPORT
#define DLL_EXPORT __declspec( dllexport )
#endif

#define PLATFORM_VIBRATE_CONTROLLER(fnName) void fnName(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed)
    typedef PLATFORM_VIBRATE_CONTROLLER(platform_vibrate_controller);
    PLATFORM_VIBRATE_CONTROLLER(platform_vibrate_controller_stub)
    {
    }
#define PLATFORM_SHUTDOWN(fnName) void fnName()
    typedef PLATFORM_SHUTDOWN(platform_shutdown);
    PLATFORM_SHUTDOWN(platform_shutdown_stub)
    {
    }
#define DEBUG_PLATFORM_TOGGLE_PAUSE(fnName) void fnName()
    typedef DEBUG_PLATFORM_TOGGLE_PAUSE(DEBUG_platform_toggle_pause);
    DEBUG_PLATFORM_TOGGLE_PAUSE(DEBUG_platform_toggle_pause_stub)
    {
    }
struct PlatformDLL
{
    platform_vibrate_controller* platformVibrateController;
    platform_shutdown* platformShutdown;
    DEBUG_platform_toggle_pause* DEBUG_platformTogglePause;
};

void platformVibrateController(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed);
void platformShutdown();

#if DEBUG
struct DEBUG_FileReadResult
{
    void* fileContents;
    uint32_t sizeBytes;
};
DEBUG_FileReadResult DEBUG_platformReadFile(const char* fileName);
bool DEBUG_platformWriteFile(const char* fileName, void* fileContents, uint32_t sizeBytes);
void DEBUG_platformFreeMemory(void* memory);
void DEBUG_platformTogglePause();
#endif
