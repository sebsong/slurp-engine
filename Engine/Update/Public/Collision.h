#pragma once
#include <set>

#define NO_OP_ON_COLLISION [](const slurp::Entity*) {}

namespace collision {
    struct CollisionShape {
        geometry::Shape shape;
        slurp::Vector2<int> offset;
    };

    // TODO: distinguish between enter and exit collision
    enum CollisionState: uint8_t {
        None,
        Colliding
    };

    struct CollisionInfo {
        bool collisionEnabled;
        bool isStatic;
        CollisionShape shape;
        std::set<slurp::Entity*> collidingWith;

        CollisionInfo();

        CollisionInfo(
            bool isStatic,
            const geometry::Shape& shape,
            bool isCentered
        );

        CollisionInfo(
            bool isStatic,
            const CollisionShape& shape
        );
    };
}
