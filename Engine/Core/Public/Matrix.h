#pragma once
#include "Vector.h"

namespace slurp {
    // Represents a homogenous matrix with an implied 3rd `z` column of:
    // [ 0 ]
    // [ 0 ]
    // [ 1 ]
    template<typename T>
    struct Mat32 {
        union {
            struct {
                T x1, y1;
                T x2, y2;
                T x3, y3;
            };

            struct {
                Vec2<T> i;
                Vec2<T> j;
                Vec2<T> k;
            };

            Vec2<T> rows[3];
        };

        Mat32(const Vec2<T>& i, const Vec2<T>& j, const Vec2<T>& k) : i(i), j(j), k(k) {}
    };
}
