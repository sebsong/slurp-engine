#pragma once
#include "Render.h"
#include "Vector.h"
#include <array>

namespace slurp {
    struct Entity {
        int size;
        render::ColorPaletteIdx color;
        float speed;
        Vector2<int> position;
        Vector2<float> positionOffset;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
        bool enabled;
        bool shouldDestroy;
    };

    struct Tilemap {
        std::array<std::array<render::ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> map;
        uint8_t tileSize;
    };
}
