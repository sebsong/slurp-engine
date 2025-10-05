#pragma once
#include <cmath>
#include <ostream>

namespace slurp {
    template<typename T>
    struct Mat22;
    template<typename T>
    struct Mat32;

    template<typename T>
    struct Vec2 {
        union {
            struct {
                T x;
                T y;
            };

            struct {
                T width;
                T height;
            };

            T values[2];
        };

        static const Vec2 Zero;
        static const uint32_t DimensionCount;

        Vec2() {
            this->x = 0;
            this->y = 0;
        }

        Vec2(T _x, T _y) {
            this->x = _x;
            this->y = _y;
        }

        bool isZero() const {
            return *this == Zero;
        }

        float magnitude() const {
            // NOTE: square roots are slow, consider magnitudeSquared
            return static_cast<float>(std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
        }

        float magnitudeSquared() const {
            return static_cast<float>(std::pow(this->x, 2) + std::pow(this->y, 2));
        }

        // TODO: this can't properly normalize an int vector. need to convert to float first
        Vec2<float> normalize() {
            float mag = magnitude();
            if (mag != 0.0f) {
                *this /= mag;
            }
            return *this;
        }

        template<typename U>
        float distanceTo(const Vec2<U>& other) const {
            return (other - *this).magnitude();
        }

        template<typename U>
        float distanceSquaredTo(const Vec2<U>& other) const {
            return (other - *this).magnitudeSquared();
        }

        template<typename U>
        bool operator==(const Vec2<U>& other) const {
            return this->x == other.x && this->y == other.y;
        }

        template<typename U>
        bool operator!=(const Vec2<U>& other) const {
            return this->x != other.x && this->y != other.y;
        }

        template<typename U>
        Vec2<std::common_type_t<T, U> > operator+(const Vec2<U>& other) const {
            return Vec2<std::common_type_t<T, U> >(this->x + other.x, this->y + other.y);
        }

        template<typename U>
        Vec2<std::common_type_t<T, U> > operator-(const Vec2<U>& other) const {
            return Vec2<std::common_type_t<T, U> >(this->x - other.x, this->y - other.y);
        }

        template<typename TScalar>
        Vec2<std::common_type_t<T, TScalar> > operator*(const TScalar& scalar) const {
            return Vec2<std::common_type_t<T, TScalar> >(this->x * scalar, this->y * scalar);
        }

        template<typename TMatrix>
        Vec2<std::common_type_t<T, TMatrix> > operator*(const Mat22<TMatrix>& matrix) const {
            return Vec2<std::common_type_t<T, TMatrix> >(
                x * matrix.x1 + y * matrix.x2,
                x * matrix.y1 + y * matrix.y2
            );
        }

        // NOTE: Implies a homogenous vector with the `z` component = 1
        template<typename TMatrix>
        Vec2<std::common_type_t<T, TMatrix> > operator*(const Mat32<TMatrix>& matrix) const {
            return Vec2<std::common_type_t<T, TMatrix> >(
                x * matrix.x1 + y * matrix.x2 + matrix.x3,
                x * matrix.y1 + y * matrix.y2 + matrix.y3
            );
        }

        template<typename TScalar>
        Vec2<std::common_type_t<T, TScalar> > operator/(const TScalar& scalar) const {
            return Vec2<std::common_type_t<T, TScalar> >(this->x / scalar, this->y / scalar);
        }

        Vec2& operator+=(const Vec2& other) {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Vec2& operator-=(const Vec2& other) {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        }

        template<typename TScalar>
        Vec2& operator*=(const TScalar& scalar) {
            this->x *= scalar;
            this->y *= scalar;
            return *this;
        }

        // NOTE: Implies a homogenous vector with the `z` component = 1
        template<typename TMatrix>
        Vec2& operator*=(const Mat32<TMatrix>& matrix) {
            this->x = x * matrix.x1 + y * matrix.x2 + matrix.x3;
            this->y = x * matrix.y1 + y * matrix.y2 + matrix.y3;
            return *this;
        }

        template<typename TScalar>
        Vec2& operator/=(const TScalar& scalar) {
            this->x = static_cast<T>(this->x / scalar);
            this->y = static_cast<T>(this->y / scalar);
            return *this;
        }

        Vec2 operator-() const {
            return Vec2(-this->x, -this->y);
        }

        template<typename TNew>
        operator Vec2<TNew>() const {
            return Vec2<TNew>(static_cast<TNew>(this->x), static_cast<TNew>(this->y));
        }
    };

    template<typename T>
    struct Vec3 {
        union {
            struct {
                T x;
                T y;
                T z;
            };

            struct {
                T r;
                T g;
                T b;
            };

            T values[3];
        };

        static const Vec3 Zero;
        static const uint32_t DimensionCount;

        Vec3() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        Vec3(T _x, T _y, T _z) {
            this->x = _x;
            this->y = _y;
            this->z = _z;
        }

        bool isZero() const {
            return *this == Zero;
        }

        float magnitude() const {
            // NOTE: square roots are slow, consider magnitudeSquared
            return static_cast<float>(
                std::sqrt(
                    std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2)
                )
            );
        }

        float magnitudeSquared() const {
            return static_cast<float>(
                std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2)
            );
        }

        // TODO: this can't properly normalize an int vector. need to convert to float first
        Vec3<float> normalize() {
            float mag = magnitude();
            if (mag != 0.0f) {
                *this /= mag;
            }
            return *this;
        }

        template<typename U>
        float distanceTo(const Vec3<U>& other) const {
            return (other - *this).magnitude();
        }

        template<typename U>
        float distanceSquaredTo(const Vec3<U>& other) const {
            return (other - *this).magnitudeSquared();
        }

        template<typename U>
        bool operator==(const Vec3<U>& other) const {
            return this->x == other.x && this->y == other.y && this->z == other.z;
        }

        template<typename U>
        bool operator!=(const Vec3<U>& other) const {
            return this->x != other.x && this->y != other.y && this->z != other.z;
        }

        template<typename U>
        Vec3<std::common_type_t<T, U> > operator+(const Vec3<U>& other) const {
            return Vec3<std::common_type_t<T, U> >(this->x + other.x, this->y + other.y, this->z + other.z);
        }

        template<typename U>
        Vec3<std::common_type_t<T, U> > operator-(const Vec3<U>& other) const {
            return Vec3<std::common_type_t<T, U> >(this->x - other.x, this->y - other.y, this->z - other.z);
        }

        template<typename TScalar>
        Vec3<std::common_type_t<T, TScalar> > operator*(const TScalar& scalar) const {
            return Vec3<std::common_type_t<T, TScalar> >(this->x * scalar, this->y * scalar, this->z * scalar);
        }

        template<typename TScalar>
        Vec3<std::common_type_t<T, TScalar> > operator/(const TScalar& scalar) const {
            return Vec3<std::common_type_t<T, TScalar> >(this->x / scalar, this->y / scalar, this->z / scalar);
        }

        Vec3& operator+=(const Vec3& other) {
            this->x += other.x;
            this->y += other.y;
            this->z += other.z;
            return *this;
        }

        Vec3& operator-=(const Vec3& other) {
            this->x -= other.x;
            this->y -= other.y;
            this->z -= other.z;
            return *this;
        }

        template<typename TScalar>
        Vec3& operator*=(const TScalar& scalar) {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
            return *this;
        }

        template<typename TScalar>
        Vec3& operator/=(const TScalar& scalar) {
            this->x = static_cast<T>(this->x / scalar);
            this->y = static_cast<T>(this->y / scalar);
            this->z = static_cast<T>(this->z / scalar);
            return *this;
        }

        Vec3 operator-() const {
            return Vec3(-this->x, -this->y, this->z);
        }

        template<typename TNew>
        operator Vec3<TNew>() const {
            return Vec3<TNew>(static_cast<TNew>(this->x), static_cast<TNew>(this->y), static_cast<TNew>(this->z));
        }
    };

    /** Constants **/
    template<typename T>
    const Vec2<T> Vec2<T>::Zero{0, 0};
    template<typename T>
    const uint32_t Vec2<T>::DimensionCount{2};
    template<typename T>
    const Vec3<T> Vec3<T>::Zero{0, 0, 0};
    template<typename T>
    const uint32_t Vec3<T>::DimensionCount{3};


    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vec2<T>& vector) {
        os << "(" << vector.x << ", " << vector.y << ")";
        return os;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vec3<T>& vector) {
        os << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
        return os;
    }
}
