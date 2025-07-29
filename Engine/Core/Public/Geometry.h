#pragma once
#include "Vector.h"
#include "Debug.h"

namespace geometry {
    enum ShapeType {
        Rect,
    };

    struct Shape {
        ShapeType type;
        slurp::Vector2<float> dimensions;
    };

    inline Shape getMinkowskiSum(const Shape& a, const Shape& b) {
        assert(a.type == Rect && b.type == Rect);
        return {Rect, a.dimensions + b.dimensions};
    }
}
