#pragma once
#include "Vector.h"

namespace slurp {
    template<typename T>
    struct Mat22 {
        union {
            struct {
                T x1, y1;
                T x2, y2;
            };

            struct {
                Vec2<T> i;
                Vec2<T> j;
            };

            Vec2<T> rows[2];

            T values[4];
        };

        Mat22(const Vec2<T>& i, const Vec2<T>& j) : i(i), j(j) {}
    };

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

            T values[6];
        };

        Mat32(const Vec2<T>& i, const Vec2<T>& j, const Vec2<T>& k) : i(i), j(j), k(k) {}
    };

    template<typename T>
    struct Mat33 {
        union {
            struct {
                T x1, y1, z1;
                T x2, y2, z2;
                T x3, y3, z3;
            };

            struct {
                Vec3<T> i;
                Vec3<T> j;
                Vec3<T> k;
            };

            Vec3<T> rows[3];

            T values[9];
        };

        Mat33(const Vec3<T>& i, const Vec3<T>& j, const Vec3<T>& k) : i(i), j(j), k(k) {}
    };
}
