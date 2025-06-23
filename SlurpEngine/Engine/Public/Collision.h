#pragma once

namespace collision {
    struct CollisionSquare {
        int radius;
    };

    // TODO: distinguish between enter and exit collision
    enum CollisionState: uint8_t {
        None,
        Colliding
    };

    struct CollisionInfo {
        bool collisionEnabled;
        bool isStatic;
#if DEBUG
        bool drawDebugCollisionShape;
#endif
        CollisionSquare collisionSquare;
        CollisionState collisionState;
        std::function<void(const slurp::Entity&)> onCollision;
    };

    CollisionSquare getMinkowskiSum(const CollisionSquare& a, const CollisionSquare& b);
}
