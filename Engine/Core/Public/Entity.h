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

        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position
        );

        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position,
            float speed,
            const collision::CollisionInfo& collisionInfo
        );

        Entity(
            std::string&& name,
            bool enabled,
            const render::RenderShape& renderShape,
            const Vector2<int>& position,
            float speed,
            const Vector2<float>& direction,
            const collision::CollisionInfo& collisionInfo
        );

        // TODO: replace with constructor version
        Entity& enableCollision(
            bool isStatic,
            const geometry::Shape shape,
            bool isCentered,
            const std::function<void(const Entity*)>& onCollisionEnter,
            const std::function<void(const Entity*)>& onCollisionExit
        ) {
            this->collisionInfo.collisionEnabled = true;
            this->collisionInfo.shape.shape = shape;
            if (isCentered) {
                this->collisionInfo.shape.offset = -shape.dimensions / 2;
            }
            this->collisionInfo.isStatic = isStatic;
            this->collisionInfo.onCollisionEnter = onCollisionEnter;
            this->collisionInfo.onCollisionExit = onCollisionExit;
            return *this;
        }

        // TODO: overload that allows you to just re-use render shape
        Entity& enableCollision(bool isStatic, const geometry::Shape shape, bool isCentered) {
            enableCollision(
                isStatic,
                shape,
                isCentered,
                NO_OP_ON_COLLISION,
                NO_OP_ON_COLLISION
            );
            return *this;
        }

        static Entity createWithoutCollision(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position
        );

        static Entity createWithCollision(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position,
            float speed,
            const collision::CollisionInfo& collisionInfo
        );

        virtual ~Entity() = default;

        bool operator==(const Entity& other) const {
            return id == other.id;
        }
    };
}
