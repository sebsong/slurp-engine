#pragma once
#include <set>

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
        std::function<void(const slurp::Entity*)> onCollisionEnter;
    };
}
