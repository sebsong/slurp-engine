#pragma once
#include "MemoryAllocator.h"

#include <deque>
#include <set>
#include <unordered_map>

namespace types {
    template<typename T, typename MemoryAllocator = memory::PermanentArenaAllocator<T> >
    using vector_arena = std::vector<T, MemoryAllocator>;

    template<typename T, typename MemoryAllocator = memory::PermanentArenaAllocator<T> >
    using set_arena = std::set<T, std::less<T>, MemoryAllocator>;

    template<typename T, typename MemoryAllocator = memory::PermanentArenaAllocator<T> >
    using deque_arena = std::deque<T, MemoryAllocator>;

    template<
        typename key_t,
        typename value_t,
        typename MemoryAllocator = memory::PermanentArenaAllocator<std::pair<const key_t, value_t> > >
    using unordered_map_arena = std::unordered_map<
        key_t,
        value_t,
        std::hash<key_t>,
        std::equal_to<key_t>,
        MemoryAllocator
    >;
}
