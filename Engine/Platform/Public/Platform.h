#pragma once
#include "DynamicDeclaration.h"
#include "MemoryConstructs.h"

#include <functional>
#include <cstdint>

namespace slurp {
    struct SlurpDll;
}

namespace platform {
#if DEBUG
    struct DEBUG_FileReadResult {
        void* fileContents;
        uint32_t sizeBytes;
    };
#endif

    slurp::SlurpDll loadSlurpLib(const char* libFilePath);

    types::byte* allocateMemory(size_t numBytes);

#define PLATFORM_GET_LOCAL_FILE_PATH(fnName) std::string fnName(const char* filename)
#define PLATFORM_VIBRATE_GAMEPAD(fnName) void fnName(int gamepadIndex, float leftMotorSpeed, float rightMotorSpeed)
#define PLATFORM_SHUTDOWN(fnName) void fnName()
#if DEBUG
#define PLATFORM_DEBUG_READ_FILE(fnName) platform::DEBUG_FileReadResult fnName(const char* fileName)
#define PLATFORM_DEBUG_WRITE_FILE(fnName) bool fnName(const char* fileName, void* fileContents, uint32_t sizeBytes)
#define PLATFORM_DEBUG_FREE_MEMORY(fnName) void fnName(void* memory)
#define PLATFORM_DEBUG_TOGGLE_PAUSE(fnName) void fnName()
#define PLATFORM_DEBUG_BEGIN_RECORDING(fnName) void fnName()
#define PLATFORM_DEBUG_END_RECORDING(fnName) void fnName()
#define PLATFORM_DEBUG_BEGIN_PLAYBACK(fnName) void fnName(const std::function<void()>& onPlaybackEnd)
#endif
    SLURP_DECLARE_DYNAMIC_RETURN(PLATFORM_GET_LOCAL_FILE_PATH, getLocalFilePath, "")

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_VIBRATE_GAMEPAD, vibrateGamepad)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_SHUTDOWN, shutdown)
#if DEBUG
    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_TOGGLE_PAUSE, DEBUG_togglePause)
#endif

    struct PlatformDll {
        dyn_getLocalFilePath* getLocalFilePath = stub_getLocalFilePath;
        dyn_vibrateGamepad* vibrateGamepad = stub_vibrateGamepad;
        dyn_shutdown* shutdown = stub_shutdown;
#if DEBUG
        dyn_DEBUG_togglePause* DEBUG_togglePause = stub_DEBUG_togglePause;
#endif
    };
}
