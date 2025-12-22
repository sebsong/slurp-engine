#include "Platform.h"
#include "SlurpEngine.h"

#include <minwindef.h>
#include <shlwapi.h>

namespace platform {
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
