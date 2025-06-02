#pragma once
#include <cstdint>

namespace math {
    inline uint8_t round(float num) {
        return static_cast<uint8_t>(num + 0.5f);
    }
}
