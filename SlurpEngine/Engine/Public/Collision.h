#pragma once

namespace collision {
    struct CollisionSquare {
        int radius;
    };

    CollisionSquare getMinkowskiSum(const CollisionSquare& a, const CollisionSquare& b);
}
