#pragma once

#include "Global.h"
#include "Types.h"
#include "SpinLock.h"

#include <string>

#define kilobytes(n) ((int64_t)n * 1024)
#define megabytes(n) (kilobytes(n) * 1024)
#define gigabytes(n) (megabytes(n) * 1024)
#define terabytes(n) (gigabytes(n) * 1024)

#define PERMANENT_ARENA_SIZE gigabytes(2)
#define SCENE_ARENA_SIZE megabytes(500)

#define TRANSIENT_ARENA_SIZE gigabytes(2)
#define SINGLE_FRAME_ARENA_SIZE megabytes(1)
#define ASSET_LOADER_ARENA_SIZE gigabytes(1)

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

        MemoryArena(std::string&& name, MemoryBlock memoryBlock);

        MemoryArena(const MemoryArena& other);

        types::byte* allocate(size_t size, bool clear = false);

        template<typename T>
        T* allocateN(size_t n, bool clear = false) {
            return reinterpret_cast<T*>(allocate(n * sizeof(T), clear));
        }

        template<typename T>
        T* allocate(bool clear = false) {
            return allocateN<T>(1, clear);
        }

        MemoryArena allocateSubArena(std::string&& name, size_t size);

        void freeAll();

        MemoryBlock getMemoryBlock() const;

        MemoryArena& operator=(const MemoryArena& newArena);

    private:
        std::string _name;
        MemoryBlock _fullMemoryBlock;
        MemoryBlock _availableMemoryBlock;
        lock::SpinLock _lock;
    };

    // TODO: doesn't make sense to split between permanent and transient anymore
    struct GameMemory {
        /** PERMANENT **/
        MemoryArena* permanent;
        MemoryArena scene;

        /** TRANSIENT **/
        MemoryArena* transient;
        MemoryArena singleFrame;
        MemoryArena assetLoader;
    };

    GLOBAL(MemoryArena* Permanent)
    GLOBAL(MemoryArena* Scene)
    GLOBAL(MemoryArena* Transient)
    GLOBAL(MemoryArena* SingleFrame)
    GLOBAL(MemoryArena* AssetLoader)
}
