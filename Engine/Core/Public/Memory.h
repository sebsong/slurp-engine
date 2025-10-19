#pragma once

#include <format>

#include "Types.h"
#include "SpinLock.h"

#include <string>

#include "Logging.h"

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
        T* allocate(size_t n) {
            return reinterpret_cast<T*>(allocate(n * sizeof(T)));
        }

        template<typename T>
        T* allocate() {
            return allocate<T>(1);
        }

        void freeAll();

        MemoryBlock getMemoryBlock() const;

    private:
        const std::string _name;
        const MemoryBlock _fullMemoryBlock;
        MemoryBlock _availableMemoryBlock;
        lock::SpinLock _lock;
    };

    struct GameMemory {
        MemoryArena permanent;
        MemoryArena transient;
    };

    static GameMemory* GlobalGameMemory;

    template<typename T>
    class MemoryArenaAllocator {
    public:
        typedef T value_type;

        MemoryArenaAllocator() = default;

        template<typename U>
        MemoryArenaAllocator(const MemoryArenaAllocator<U>& allocator) {
            this->arena = allocator.arena;
        }

        T* allocate(size_t n) {
            logging::debug(std::format("ALLOCATE: {}", n));
            return arena->allocate<T>(n); // TODO: respect memory alignment
        }

        void deallocate(T* ptr, size_t n) {
            logging::debug(std::format("DE-ALLOCATE: {}", n));
            // NOTE: rely on manual, bulk free instead of deallocating each pointer
        }

        MemoryArena* arena;
    };

    template<typename T>
    class PermanentArenaAllocator : public MemoryArenaAllocator<T> {
    public:
        PermanentArenaAllocator(): MemoryArenaAllocator<T>() {
            this->arena = &GlobalGameMemory->permanent;
        }

        template<typename U>
        PermanentArenaAllocator(const PermanentArenaAllocator<U>& allocator): MemoryArenaAllocator<T>(allocator) {}
    };

    template<typename T>
    class TransientArenaAllocator : public MemoryArenaAllocator<T> {
    public:
        TransientArenaAllocator(): MemoryArenaAllocator<T>() {
            this->arena = &GlobalGameMemory->transient;
        }

        template<typename U>
        TransientArenaAllocator(const TransientArenaAllocator<U>& allocator): MemoryArenaAllocator<T>(allocator) {}
    };
}
