#pragma once
#include "Vector.h"
#include "Debug.h"

namespace geometry {
    enum ShapeType {
        Rect,
        Square,
    };

    struct Shape {
        ShapeType type;
        slurp::Vector2<int> dimensions;
    };

    inline Shape getMinkowskiSum(const Shape& a, const Shape& b) {
        assert(a.type == Square && b.type == Square);
        if (a.type == Square && b.type == Square) {
            return {Square, a.dimensions + b.dimensions};
        }
        return {};
    }
}
