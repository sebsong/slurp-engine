#include "Platform.h"
#include "SlurpEngine.h"

#include <dlfcn.h>
#include <filesystem>
#include <mach-o/dyld.h>

namespace platform {
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

        void* libHandle = dlopen(libFilePath, RTLD_NOW);
        macLoadLibFn<slurp::dyn_init>(
            slurpLib.init,
            "init",
            slurp::stub_init,
            libHandle
        );
        macLoadLibFn<slurp::dyn_frameStart>(
            slurpLib.frameStart,
            "frameStart",
            slurp::stub_frameStart,
            libHandle
        );
        macLoadLibFn<slurp::dyn_handleInput>(
            slurpLib.handleInput,
            "handleInput",
            slurp::stub_handleInput,
            libHandle
        );
        macLoadLibFn<slurp::dyn_bufferAudio>(
            slurpLib.bufferAudio,
            "bufferAudio",
            slurp::stub_bufferAudio,
            libHandle
        );
        macLoadLibFn<slurp::dyn_updateAndRender>(
            slurpLib.updateAndRender,
            "updateAndRender",
            slurp::stub_updateAndRender,
            libHandle
        );
        macLoadLibFn<slurp::dyn_frameEnd>(
            slurpLib.frameEnd,
            "frameEnd",
            slurp::stub_frameEnd,
            libHandle
        );
        macLoadLibFn<slurp::dyn_shutdown>(
            slurpLib.shutdown,
            "shutdown",
            slurp::stub_shutdown,
            libHandle
        );
        dlclose(libHandle);

        return slurpLib;
    }

    types::byte* allocateMemory(size_t numBytes) {
        return static_cast<types::byte*>(calloc(numBytes, sizeof(types::byte)));
    }
}
