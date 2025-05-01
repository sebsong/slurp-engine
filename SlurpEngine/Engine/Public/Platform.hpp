#pragma once
#include <cstdint>

#define DLL_EXPORT
#if PLATFORM_WINDOWS
#undef DLL_EXPORT
#define DLL_EXPORT __declspec( dllexport )
#endif

// NOTE: Define dynamic types for hot reloading
#define NO_ARG
#define _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, fnPrefix, stubReturn) \
    typedef fnMacro(dyn_##fnName);                                    \
    fnMacro(stub_##fnName){ stubReturn }                              \
    fnPrefix fnMacro(fnName);
#define SLURP_DECLARE_DYNAMIC_VOID(fnMacro, fnName) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, NO_ARG, NO_ARG)
#define SLURP_DECLARE_DYNAMIC_RETURN(fnMacro, fnName, stubReturn) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, NO_ARG, return stubReturn##;)
#define SLURP_DECLARE_DYNAMIC_DLL_VOID(fnMacro, fnName) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, extern "C" DLL_EXPORT, NO_ARG)

namespace platform
{
#if DEBUG
    struct DEBUG_FileReadResult
    {
        void* fileContents;
        uint32_t sizeBytes;
    };
#endif

#define PLATFORM_VIBRATE_CONTROLLER(fnName) void fnName(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed)
#define PLATFORM_SHUTDOWN(fnName) void fnName()
#if DEBUG
#define PLATFORM_DEBUG_READ_FILE(fnName) platform::DEBUG_FileReadResult fnName(const char* fileName)
#define PLATFORM_DEBUG_WRITE_FILE(fnName) bool fnName(const char* fileName, void* fileContents, uint32_t sizeBytes)
#define PLATFORM_DEBUG_FREE_MEMORY(fnName) void fnName(void* memory)
#define PLATFORM_DEBUG_TOGGLE_PAUSE(fnName) void fnName()
#endif

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_VIBRATE_CONTROLLER, vibrateController)
    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_SHUTDOWN, shutdown)
#if DEBUG
    SLURP_DECLARE_DYNAMIC_RETURN(PLATFORM_DEBUG_READ_FILE, DEBUG_readFile, platform::DEBUG_FileReadResult())
    SLURP_DECLARE_DYNAMIC_RETURN(PLATFORM_DEBUG_WRITE_FILE, DEBUG_writeFile, false)
    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_FREE_MEMORY, DEBUG_freeMemory)
    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_TOGGLE_PAUSE, DEBUG_togglePause)
#endif

    struct PlatformDll
    {
        dyn_vibrateController* vibrateController = stub_vibrateController;
        dyn_shutdown* shutdown = stub_shutdown;
#if DEBUG
        dyn_DEBUG_readFile* DEBUG_readFile = stub_DEBUG_readFile;
        dyn_DEBUG_writeFile* DEBUG_writeFile = stub_DEBUG_writeFile;
        dyn_DEBUG_freeMemory* DEBUG_freeMemory = stub_DEBUG_freeMemory;
        dyn_DEBUG_togglePause* DEBUG_togglePause = stub_DEBUG_togglePause;
#endif
    };
}
