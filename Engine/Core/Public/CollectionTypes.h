#pragma once
#include <unordered_map>

#include "MemoryAllocator.h"

namespace types {
    template<typename key_t, typename value_t>
    using unordered_map_arena =
    std::unordered_map<
        key_t,
        value_t,
        std::hash<key_t>,
        std::equal_to<key_t>,
        memory::PermanentArenaAllocator<std::pair<const key_t, value_t> >
    >;
}
