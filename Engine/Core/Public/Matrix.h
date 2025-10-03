#pragma once
#include "Vector.h"

namespace slurp {
    template<typename T>
    struct Mat32<T> {
        union {
            struct {
                T x1, y1;
                T x2, y2;
                T x3, y3;
            };

            Vec2<T> rows[3];
        };
    };
}
