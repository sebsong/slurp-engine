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

    CollisionSquare getMinkowskiSum(const CollisionSquare& a, const CollisionSquare& b);
}
