#pragma once

#include "Memory.h"
#include "Logging.h"

#include <format>

namespace memory {
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
            return arena->allocate<T>(n); // TODO: respect memory alignment
        }

        void deallocate(T* ptr, size_t n) {
            // NOTE: rely on manual, bulk free instead of deallocating each pointer
        }

        MemoryArena* arena;
    };

    template<typename T>
    class PermanentArenaAllocator : public MemoryArenaAllocator<T> {
    public:
        PermanentArenaAllocator(): MemoryArenaAllocator<T>() {
            this->arena = &GlobalGameMemory.permanent;
        }

        template<typename U>
        PermanentArenaAllocator(const PermanentArenaAllocator<U>& allocator): MemoryArenaAllocator<T>(allocator) {}
    };

    template<typename T>
    class TransientArenaAllocator : public MemoryArenaAllocator<T> {
    public:
        TransientArenaAllocator(): MemoryArenaAllocator<T>() {
            this->arena = &GlobalGameMemory.transient;
        }

        template<typename U>
        TransientArenaAllocator(const TransientArenaAllocator<U>& allocator): MemoryArenaAllocator<T>(allocator) {}
    };
}
