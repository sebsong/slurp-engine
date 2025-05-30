﻿#pragma once
#include <Render.hpp>
#include <Vector.hpp>
#include <array>

namespace slurp
{
    struct Entity
    {
        bool enabled;
        int size;
        render::ColorPaletteIdx color;
        float speed;
        Vector2<int> position;
        Vector2<float> positionOffset;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
    };

    struct Tilemap
    {
        std::array<std::array<render::ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> map;
        uint8_t tileSize;
    };

}
