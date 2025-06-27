#pragma once
#include "Vector.h"
#include "Render.h"
#include <functional>

#include "Collision.h"

namespace slurp {
    struct Entity {
        uint32_t id;
        std::string name;
        bool enabled;
        render::RenderShape renderShape;
        Vector2<int> position;
        Vector2<float> renderOffset;
        float speed;
        Vector2<float> direction;
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        Entity& enableCollision(bool isStatic, int radius, const std::function<void(const Entity&)>& onCollision) {
            this->collisionInfo.collisionEnabled = true;
            this->collisionInfo.collisionSquare.radius = radius;
            this->collisionInfo.isStatic = isStatic;
            this->collisionInfo.onCollision = onCollision;
            return *this;
        }

        Entity& enableCollision(bool isStatic, int radius) {
            enableCollision(isStatic, radius, [](const Entity&) {});
            return *this;
        }

        bool operator==(const Entity& other) const {
            return id == other.id;
        }
    };
}
