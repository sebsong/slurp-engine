#pragma once
#include <cstdint>

#define DLL_EXPORT
#if PLATFORM_WINDOWS
#undef DLL_EXPORT
#define DLL_EXPORT __declspec( dllexport )
#endif

// NOTE: Define dynamic types for hot reloading
// TODO: allow for an optional stub return value
// TODO: make extern and dll export optional
#define SLURP_DECLARE_DYNAMIC(fnMacro, fnName) \
    typedef fnMacro(dyn_##fnName);             \
    fnMacro(stub_##fnName){}                   \
    extern "C" DLL_EXPORT fnMacro(fnName);

namespace platform
{
#define PLATFORM_VIBRATE_CONTROLLER(fnName) void fnName(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed)
#define PLATFORM_SHUTDOWN(fnName) void fnName()

    SLURP_DECLARE_DYNAMIC(PLATFORM_VIBRATE_CONTROLLER, vibrateController)
    SLURP_DECLARE_DYNAMIC(PLATFORM_SHUTDOWN, shutdown)


#if DEBUG
    struct DEBUG_FileReadResult
    {
        void* fileContents;
        uint32_t sizeBytes;
    };

#define PLATFORM_DEBUG_TOGGLE_PAUSE(fnName) void fnName()

    //TODO: make these dynamic
    DEBUG_FileReadResult DEBUG_readFile(const char* fileName);
    bool DEBUG_writeFile(const char* fileName, void* fileContents, uint32_t sizeBytes);
    void DEBUG_freeMemory(void* memory);
    SLURP_DECLARE_DYNAMIC(PLATFORM_DEBUG_TOGGLE_PAUSE, DEBUG_togglePause)
#endif

    struct PlatformDll
    {
        dyn_vibrateController* vibrateController = stub_vibrateController;
        dyn_shutdown* shutdown = stub_shutdown;
#if DEBUG
        dyn_DEBUG_togglePause* DEBUG_togglePause = stub_DEBUG_togglePause;
#endif
    };
}
