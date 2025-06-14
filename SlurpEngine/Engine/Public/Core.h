#pragma once
#include "Render.h"
#include "Vector.h"
#include <array>

#include "Update.h"

namespace slurp {
    struct Entity {
        int size;
        render::ColorPaletteIdx color;
        float speed;
        Vector2<int> position;
        Vector2<float> positionOffset;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
        update::CollisionSquare collisionSquare;
        bool enabled;
        bool collisionEnabled;
        bool shouldDestroy;

        // TODO: should probably just have a unique entity id to compare
        bool operator==(const Entity& other) const {
            return position == other.position;
        }

    };

    struct Tilemap {
        std::array<std::array<render::ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> map;
        uint8_t tileSize;
    };
}
