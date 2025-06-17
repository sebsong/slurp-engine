#include "Collision.h"

namespace collision {
    CollisionSquare getMinkowskiSum(const CollisionSquare& a, const CollisionSquare& b) {
        return CollisionSquare{a.radius + b.radius};
    }
}
