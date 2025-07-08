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
        float speed;
        Vector2<float> direction;
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        Entity& enableCollision(
            bool isStatic,
            const geometry::Shape shape,
            bool centerPosition,
            const std::function<void(const Entity*)>& onCollision
        ) {
            this->collisionInfo.collisionEnabled = true;
            this->collisionInfo.shape.shape = shape;
            if (centerPosition) {
                this->collisionInfo.shape.offset = -shape.dimensions / 2;
            }
            this->collisionInfo.isStatic = isStatic;
            this->collisionInfo.onCollision = onCollision;
            return *this;
        }

        // TODO: overload that allows you to just re-use render shape
        Entity& enableCollision(bool isStatic, const geometry::Shape shape, bool centerPosition) {
            enableCollision(isStatic, shape, centerPosition, [](const Entity*) {});
            return *this;
        }

        bool operator==(const Entity& other) const {
            return id == other.id;
        }
    };
}
