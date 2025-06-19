#pragma once
#include "Render.h"
#include "Vector.h"
#include <array>

#include "Collision.h"

namespace slurp {
    struct Entity {
        int id;
        std::string name;
        bool enabled;
        render::ColorPaletteIdx color;
        int size;
        Vector2<int> position;
        Vector2<float> renderOffset;
        float speed;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4]; // TODO: get rid of this
        bool collisionEnabled;
        bool isStatic;
        collision::CollisionSquare collisionSquare;
#if DEBUG
        bool drawDebugCollisionShape;
#endif
        bool shouldDestroy;

        void enableCollision(bool isStatic) {
            this->collisionEnabled = true;
            this->collisionSquare.radius = this->size / 2;
            this->isStatic = isStatic;
        }

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
