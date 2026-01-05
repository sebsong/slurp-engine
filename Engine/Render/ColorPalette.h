#pragma once
#include "Vector.h"

#define COLOR_PALETTE_SIZE 9

namespace render {
    struct ColorPalette {
        slurp::Vec3<float> colors[COLOR_PALETTE_SIZE];
    };
}
