#include "Platform.h"
#include "SlurpEngine.h"

#include <minwindef.h>
#include <shlwapi.h>

namespace platform {
    static HMODULE GlobalSlurpLibHandle;

    template<typename T>
    static void winLoadLibFn(T*& out, const char* fnName, T* stubFn, HMODULE libHandle) {
        out = reinterpret_cast<T*>(
            GetProcAddress(libHandle, fnName)
        );
        if (!out) {
            ASSERT_LOG(out, std::format("Failed to load slurp lib function: {}.", fnName));
            out = stubFn;
        }
    }

    slurp::SlurpDll loadSlurpLib(const char* libFilePath) {
        slurp::SlurpDll slurpLib;

        if (GlobalSlurpLibHandle) {
            FreeLibrary(GlobalSlurpLibHandle);
        }
        GlobalSlurpLibHandle = LoadLibraryA(libFilePath);
        winLoadLibFn<slurp::dyn_slurp_init>(
            slurpLib.init,
            "slurp_init",
            slurp::stub_slurp_init,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_frameStart>(
            slurpLib.frameStart,
            "slurp_frameStart",
            slurp::stub_slurp_frameStart,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_handleInput>(
            slurpLib.handleInput,
            "slurp_handleInput",
            slurp::stub_slurp_handleInput,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_bufferAudio>(
            slurpLib.bufferAudio,
            "slurp_bufferAudio",
            slurp::stub_slurp_bufferAudio,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_updateAndRender>(
            slurpLib.updateAndRender,
            "slurp_updateAndRender",
            slurp::stub_slurp_updateAndRender,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_frameEnd>(
            slurpLib.frameEnd,
            "slurp_frameEnd",
            slurp::stub_slurp_frameEnd,
            GlobalSlurpLibHandle
        );
        winLoadLibFn<slurp::dyn_slurp_shutdown>(
            slurpLib.shutdown,
            "slurp_shutdown",
            slurp::stub_slurp_shutdown,
            GlobalSlurpLibHandle
        );
        return slurpLib;
    }

    types::byte* allocateMemory(size_t numBytes) {
#if DEBUG
        void* baseAddress = (void*) terabytes(1);
#else
        void* baseAddress = nullptr;
#endif
        return static_cast<types::byte*>(VirtualAlloc(
            baseAddress,
            numBytes,
            MEM_RESERVE | MEM_COMMIT,
            // TODO: could we use MEM_LARGE_PAGES to alleviate TLB
            PAGE_READWRITE
        ));
    }
}
