#include "Platform.h"
#include "SlurpEngine.h"

#include <dlfcn.h>
#include <mach-o/dyld.h>

namespace platform {
    types::byte* allocateMemory(size_t numBytes) {
        return static_cast<types::byte*>(calloc(numBytes, sizeof(types::byte)));
    }
}
