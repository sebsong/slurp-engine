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
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        void enableCollision(bool isStatic, const std::function<void(const Entity&)>& onCollision) {
            this->collisionInfo.collisionEnabled = true;
            this->collisionInfo.collisionSquare.radius = this->size / 2;
            this->collisionInfo.isStatic = isStatic;
            this->collisionInfo.onCollision = onCollision;
        }

        void enableCollision(bool isStatic) {
            enableCollision(isStatic, [](const Entity&){});
        }

        bool operator==(const Entity& other) const {
            return id == other.id;
        }

    };
}
