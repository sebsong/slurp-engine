#pragma once
#include "Vector.h"

namespace geometry {
    enum ShapeType {
        Rect,
        Square,
    };

    struct Shape {
        ShapeType type;
        slurp::Vector2<int> dimensions;
    };
}
