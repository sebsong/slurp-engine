#pragma once
#include "MemoryAllocator.h"

#include <deque>
#include <set>
#include <unordered_map>

namespace types {
    template<typename T>
    using set_arena = std::set<T, std::less<T>, memory::PermanentArenaAllocator<T> >;

    template<typename T>
    using deque_arena = std::deque<T, memory::PermanentArenaAllocator<T> >;

    template<typename key_t, typename value_t>
    using unordered_map_arena = std::unordered_map<
        key_t,
        value_t,
        std::hash<key_t>,
        std::equal_to<key_t>,
        memory::PermanentArenaAllocator<std::pair<const key_t, value_t> >
    >;

    template<typename T>
    using hash_arena = std::hash<memory::PermanentArenaAllocator<T> >;
}
