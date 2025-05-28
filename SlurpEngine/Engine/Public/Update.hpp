#pragma once
#include <Vector.hpp>
#include "Render.hpp"

#define EMPTY_COLOR_PALETTE_IDX 6

namespace slurp
{
    struct Entity
    {
        bool enabled = true;
        int size;
        ColorPaletteIdx color;
        float speed;
        Vector2<int> position;
        Vector2<float> positionOffset;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
    };

    struct Tilemap;
    void updatePosition(Entity& entity, const Tilemap& tilemap, const float& dt);
}
