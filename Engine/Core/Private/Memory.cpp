#include "Memory.h"

#include "Debug.h"

namespace memory {
    MemoryArena::MemoryArena(): _name("UNINITIALIZED"),
                                _fullMemoryBlock({}),
                                _availableMemoryBlock({}) {}

    MemoryArena::MemoryArena(
        std::string&& name,
        types::byte* memory,
        size_t size
    ): _name(std::move(name)),
       _fullMemoryBlock({memory, size}),
       _availableMemoryBlock({memory, size}) {}

    types::byte* MemoryArena::allocate(size_t size) {
        _lock.lock();

        ASSERT(size <= _availableMemoryBlock.size);
        if (size > _availableMemoryBlock.size) {
            _lock.release();
            return nullptr;
        }

        types::byte* memory = _availableMemoryBlock.memory;
        _availableMemoryBlock.size -= size;
        _availableMemoryBlock.memory += size;

        _lock.release();
        return memory;
    }


    void MemoryArena::freeAll() {
        _availableMemoryBlock = _fullMemoryBlock;
    }

    MemoryBlock MemoryArena::getMemoryBlock() const {
        return _fullMemoryBlock;
    }
}
