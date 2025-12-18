#pragma once
#include "CollectionTypes.h"
#include "Geometry.h"

#include <set>

namespace entity {
    struct Entity;
}

namespace collision {
    struct CollisionShape {
        geometry::Shape shape;
        slurp::Vec2<float> offset;

        bool hitTest(const slurp::Vec2<float>& location) const {
            return shape.hitTest(location - offset);
        };
    };

    // TODO: distinguish between enter and exit collision
    enum CollisionState: uint8_t {
        None,
        Colliding
    };

    struct CollisionInfo {
        bool collisionEnabled;
        bool isStatic;
        bool isTrigger;
        CollisionShape shape;
        types::set_arena<entity::Entity*> collidingWith;

        CollisionInfo();

        CollisionInfo(
            bool isStatic,
            bool isTrigger,
            const geometry::Shape& shape,
            bool isCentered
        );

        CollisionInfo(
            bool isStatic,
            bool isTrigger,
            const CollisionShape& shape
        );
    };

    struct CollisionDetails {
        entity::Entity* entity;
        // TODO: add a collision normal and other collision info
    };

    void handleCollisionEnter(entity::Entity* entity, entity::Entity* otherEntity);

    void handleCollisionExit(entity::Entity* entity, entity::Entity* otherEntity);
}
