#pragma once
#include <cstdint>

#define DLL_EXPORT
#if PLATFORM_WINDOWS
#undef DLL_EXPORT
#define DLL_EXPORT __declspec( dllexport )
#endif

// NOTE: Define dynamic types for hot reloading
#define SLURP_DECLARE_DYNAMIC(fnMacro, fnName) \
    typedef fnMacro(dyn_##fnName);             \
    fnMacro(stub_##fnName){}                   \
    extern "C" DLL_EXPORT fnMacro(fnName);

namespace platform
{
#define PLATFORM_VIBRATE_CONTROLLER(fnName) void fnName(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed)
#define PLATFORM_SHUTDOWN(fnName) void fnName()
#define DEBUG_PLATFORM_TOGGLE_PAUSE(fnName) void fnName()

    SLURP_DECLARE_DYNAMIC(PLATFORM_VIBRATE_CONTROLLER, platformVibrateController)
    SLURP_DECLARE_DYNAMIC(PLATFORM_SHUTDOWN, platformShutdown)
    SLURP_DECLARE_DYNAMIC(DEBUG_PLATFORM_TOGGLE_PAUSE, DEBUG_platformTogglePause)

    struct PlatformDll
    {
        dyn_platformVibrateController* platformVibrateController = stub_platformVibrateController;
        dyn_platformShutdown* platformShutdown = stub_platformShutdown;
        dyn_DEBUG_platformTogglePause* DEBUG_platformTogglePause = stub_DEBUG_platformTogglePause;
    };

#if DEBUG
    struct DEBUG_FileReadResult
    {
        void* fileContents;
        uint32_t sizeBytes;
    };

    //TODO: make these dynamic

    DEBUG_FileReadResult DEBUG_platformReadFile(const char* fileName);
    bool DEBUG_platformWriteFile(const char* fileName, void* fileContents, uint32_t sizeBytes);
    void DEBUG_platformFreeMemory(void* memory);
    // void DEBUG_platformTogglePause();
#endif
}
