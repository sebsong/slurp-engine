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

    void loadSlurpLib(const char* libFilePath, slurp::SlurpDll& outSlurpLib);

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

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_VIBRATE_GAMEPAD, vibrateGamepad)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_SHUTDOWN, shutdown)
#if DEBUG
    SLURP_DECLARE_DYNAMIC_RETURN(PLATFORM_DEBUG_READ_FILE, DEBUG_readFile, platform::DEBUG_FileReadResult())

    SLURP_DECLARE_DYNAMIC_RETURN(PLATFORM_DEBUG_WRITE_FILE, DEBUG_writeFile, false)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_FREE_MEMORY, DEBUG_freeMemory)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_TOGGLE_PAUSE, DEBUG_togglePause)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_BEGIN_RECORDING, DEBUG_beginRecording)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_END_RECORDING, DEBUG_endRecording)

    SLURP_DECLARE_DYNAMIC_VOID(PLATFORM_DEBUG_BEGIN_PLAYBACK, DEBUG_beginPlayback)
#endif

    // TODO: ASSERT that we actually replace these stubs properly
    struct PlatformDll {
        dyn_vibrateGamepad* vibrateGamepad = stub_vibrateGamepad;
        dyn_shutdown* shutdown = stub_shutdown;
#if DEBUG
        dyn_DEBUG_readFile* DEBUG_readFile = stub_DEBUG_readFile;
        dyn_DEBUG_writeFile* DEBUG_writeFile = stub_DEBUG_writeFile;
        dyn_DEBUG_freeMemory* DEBUG_freeMemory = stub_DEBUG_freeMemory;
        dyn_DEBUG_togglePause* DEBUG_togglePause = stub_DEBUG_togglePause;
        dyn_DEBUG_beginRecording* DEBUG_beginRecording = stub_DEBUG_beginRecording;
        dyn_DEBUG_endRecording* DEBUG_endRecording = stub_DEBUG_endRecording;
        dyn_DEBUG_beginPlayback* DEBUG_beginPlayback = stub_DEBUG_beginPlayback;
#endif
    };
}
