#pragma once
#include "Vector.h"
#include "Debug.h"

namespace geometry {
    enum ShapeType {
        Rect,
    };

    struct Shape {
        ShapeType type;
        slurp::Vector2<int> dimensions;
    };

    inline Shape getMinkowskiSum(const Shape& a, const Shape& b) {
        return {Rect, a.dimensions + b.dimensions};
    }
}
