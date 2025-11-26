#include "Platform.h"
#include "SlurpEngine.h"

#include <dlfcn.h>
#include <mach-o/dyld.h>

namespace platform {
    std::string getLocalFilePath(const char* fileName) {
        char buf[PATH_MAX];
        uint32_t bufferSize = PATH_MAX;
        if (_NSGetExecutablePath(buf, &bufferSize) == 0) {
            logging::debug(std::string(buf) + "/" + fileName);
        }
        return std::string(buf) + "/" + fileName;
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

    void loadSlurpLib(const char* libFilePath, slurp::SlurpDll& outSlurpLib) {
        void* libHandle = dlopen(libFilePath, RTLD_NOW);
        macLoadLibFn<slurp::dyn_init>(
            outSlurpLib.init,
            "init",
            slurp::stub_init,
            libHandle
        );
        macLoadLibFn<slurp::dyn_frameStart>(
            outSlurpLib.frameStart,
            "frameStart",
            slurp::stub_frameStart,
            libHandle
        );
        macLoadLibFn<slurp::dyn_handleInput>(
            outSlurpLib.handleInput,
            "handleInput",
            slurp::stub_handleInput,
            libHandle
        );
        macLoadLibFn<slurp::dyn_bufferAudio>(
            outSlurpLib.bufferAudio,
            "bufferAudio",
            slurp::stub_bufferAudio,
            libHandle
        );
        macLoadLibFn<slurp::dyn_updateAndRender>(
            outSlurpLib.updateAndRender,
            "updateAndRender",
            slurp::stub_updateAndRender,
            libHandle
        );
        macLoadLibFn<slurp::dyn_frameEnd>(
            outSlurpLib.frameEnd,
            "frameEnd",
            slurp::stub_frameEnd,
            libHandle
        );
        macLoadLibFn<slurp::dyn_shutdown>(
            outSlurpLib.shutdown,
            "shutdown",
            slurp::stub_shutdown,
            libHandle
        );
        dlclose(libHandle);
    }
}
