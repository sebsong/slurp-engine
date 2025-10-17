#pragma once

#include "Types.h"
#include "SpinLock.h"

#include <string>

namespace memory {
    /**
     * NOTE: Some useful memory constructs
     *
     * Memory Arena: big block section of memory, allocate many times like stack, deallocate all at once.
     * Can have separate/sub arenas for different tasks. Gains efficiency by having context of the specific task's
     * memory footprint/lifecycle.
     *
     * Memory Pool: section of memory for a collection of fixed size objects, useful for gracefully limiting the
     * number of object instances.
     *
     * "Dynamic" Free List Tracking Memory: Keep track of free memory subsections, individually allocate and
     * de-allocate variable sized objects, flexible for varying sizes and memory lifecycles, susceptible to fragmentation
     *
     **/

    struct MemoryBlock {
        types::byte* memory;
        size_t size;
    };

    class MemoryArena {
    public:
        MemoryArena();

        MemoryArena(std::string&& name, types::byte* memory, size_t size);

        types::byte* allocate(size_t size);

        template<typename T>
        T* allocate() {
            return reinterpret_cast<T*>(allocate(sizeof(T)));
        }

        void freeAll();

        MemoryBlock getMemoryBlock() const;

    private:
        const std::string _name;
        const MemoryBlock _fullMemoryBlock;
        MemoryBlock _availableMemoryBlock;
        lock::SpinLock _lock;
    };
}
