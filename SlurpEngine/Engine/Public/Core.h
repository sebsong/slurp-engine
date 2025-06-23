#pragma once
#include "Render.h"
#include "Vector.h"
#include <functional>

#include "Collision.h"

namespace slurp {
    struct Entity {
        uint32_t id;
        std::string name;
        bool enabled;
        render::ColorPaletteIdx color;
        int size;
        Vector2<int> position;
        Vector2<float> renderOffset;
        float speed;
        Vector2<float> direction;
        bool collisionEnabled;
        bool isStatic;
        collision::CollisionSquare collisionSquare;
        std::function<void(const Entity&)> onCollision;
        collision::CollisionState collisionState;
#if DEBUG
        bool drawDebugCollisionShape;
#endif
        bool shouldDestroy;

        void enableCollision(bool isStatic, const std::function<void(const Entity&)>& onCollision) {
            this->collisionEnabled = true;
            this->collisionSquare.radius = this->size / 2;
            this->isStatic = isStatic;
            this->onCollision = onCollision;
        }

        void enableCollision(bool isStatic) {
            enableCollision(isStatic, [](const Entity&){});
        }

        // TODO: should probably just have a unique entity id to compare
        bool operator==(const Entity& other) const {
            return position == other.position;
        }

    };
}
