#pragma once

#include "Vector.h"

#include <cstdint>

namespace entity {
    struct Entity;
}

namespace render {
    typedef uint32_t Pixel;

    constexpr Pixel AlphaMask = 0xFF000000;
    constexpr uint8_t AlphaShift = 24;
    constexpr Pixel RedMask = 0x00FF0000;
    constexpr uint8_t RedShift = 16;
    constexpr Pixel GreenMask = 0x0000FF00;
    constexpr uint8_t GreenShift = 8;
    constexpr Pixel BlueMask = 0x000000FF;
    constexpr uint8_t BlueShift = 0;
}
