#include "Platform.h"
#include "SlurpEngine.h"

#include <dlfcn.h>
#include <filesystem>
#include <mach-o/dyld.h>

namespace platform {
    static void* slurpLibHandle;

    std::string getLocalFilePath(const char* fileName) {
        char filePath[PATH_MAX];
        uint32_t bufferSize = PATH_MAX;
        if (_NSGetExecutablePath(filePath, &bufferSize) == 0) {
            return std::filesystem::path(filePath).replace_filename(fileName).string();
        }
        ASSERT_LOG(false, "Failed to get local file path.");
        return "";
    }

    template<typename T>
    static void macLoadLibFn(T*& out, const char* fnName, T* stubFn, void* libHandle) {
        out = reinterpret_cast<T*>(
            dlsym(libHandle, fnName)
        );
        if (!out) {
            ASSERT_LOG(out, std::format("Failed to load slurp lib function: {}.", fnName));
            out = stubFn;
        }
    }

    slurp::SlurpDll loadSlurpLib(const char* libFilePath) {
        slurp::SlurpDll slurpLib;

        if (slurpLibHandle) {
            dlclose(slurpLibHandle);
        }
        slurpLibHandle = dlopen(libFilePath, RTLD_NOW);
        macLoadLibFn<slurp::dyn_slurp_init>(
            slurpLib.init,
            "slurp_init",
            slurp::stub_slurp_init,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_frameStart>(
            slurpLib.frameStart,
            "slurp_frameStart",
            slurp::stub_slurp_frameStart,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_handleInput>(
            slurpLib.handleInput,
            "slurp_handleInput",
            slurp::stub_slurp_handleInput,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_bufferAudio>(
            slurpLib.bufferAudio,
            "slurp_bufferAudio",
            slurp::stub_slurp_bufferAudio,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_updateAndRender>(
            slurpLib.updateAndRender,
            "slurp_updateAndRender",
            slurp::stub_slurp_updateAndRender,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_frameEnd>(
            slurpLib.frameEnd,
            "slurp_frameEnd",
            slurp::stub_slurp_frameEnd,
            slurpLibHandle
        );
        macLoadLibFn<slurp::dyn_slurp_shutdown>(
            slurpLib.shutdown,
            "slurp_shutdown",
            slurp::stub_slurp_shutdown,
            slurpLibHandle
        );
        return slurpLib;
    }

    types::byte* allocateMemory(size_t numBytes) {
        return static_cast<types::byte*>(calloc(numBytes, sizeof(types::byte)));
    }
}
