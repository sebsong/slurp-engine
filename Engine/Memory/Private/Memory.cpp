#include "Memory.h"

#include "Debug.h"

namespace memory {
    MemoryArena::MemoryArena(): _name("UNINITIALIZED"),
                                _fullMemoryBlock({}),
                                _availableMemoryBlock({}) {}

    MemoryArena::MemoryArena(
        std::string&& name,
        MemoryBlock memoryBlock
    ): _name(std::move(name)),
       _fullMemoryBlock(memoryBlock),
       _availableMemoryBlock(memoryBlock) {}

    MemoryArena::MemoryArena(
        const MemoryArena& other
    ): _name(other._name),
       _fullMemoryBlock(other._fullMemoryBlock),
       _availableMemoryBlock(other._availableMemoryBlock) {}

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

    MemoryArena MemoryArena::allocateSubArena(std::string&& name, size_t size) {
        MemoryBlock memoryBlock = {allocate(size), size};
        return MemoryArena(std::move(name), memoryBlock);
    }

    void MemoryArena::freeAll() {
        _availableMemoryBlock = _fullMemoryBlock;
    }

    MemoryBlock MemoryArena::getMemoryBlock() const {
        return _fullMemoryBlock;
    }

    MemoryArena& MemoryArena::operator=(const MemoryArena& newArena) {
        _name = newArena._name;
        _fullMemoryBlock = newArena._fullMemoryBlock;
        _availableMemoryBlock = newArena._availableMemoryBlock;
        return *this;
    }
}
