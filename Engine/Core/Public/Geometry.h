#pragma once
#include "Vector.h"
#include "Debug.h"
#include "Mathematics.h"

namespace geometry {
    enum ShapeType {
        Rect,
    };

    struct Shape {
        ShapeType type;
        slurp::Vec2<float> dimensions;

        bool hitTest(const slurp::Vec2<float>& location) const {
            ASSERT(type == Rect);
            return math::inRange(location.x, 0.f, dimensions.x) &&
                   math::inRange(location.y, 0.f, dimensions.y);
        }
    };


    inline Shape getMinkowskiSum(const Shape& a, const Shape& b) {
        ASSERT(a.type == Rect && b.type == Rect);
        return {Rect, a.dimensions + b.dimensions};
    }
}
