#pragma once
#include <set>

#include "Geometry.h"

namespace slurp {
    struct Entity;
}

#define NO_OP_ON_COLLISION [](const slurp::Entity*) {}

namespace collision {
    struct CollisionShape {
        geometry::Shape shape;
        slurp::Vec2<float> offset;
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
        types::set_arena<slurp::Entity*> collidingWith;

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
        slurp::Entity* entity;
        // TODO: add a collision normal and other collision info
    };

    void handleCollisionEnter(slurp::Entity* entity, slurp::Entity* otherEntity);

    void handleCollisionExit(slurp::Entity* entity, slurp::Entity* otherEntity);
}
