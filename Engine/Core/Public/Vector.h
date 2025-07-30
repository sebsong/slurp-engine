#pragma once
#include <cmath>
#include <ostream>

namespace slurp {
    template<typename T>
    struct Vec2 {
        T x;
        T y;

        static const Vec2 Zero;
        static const Vec2 Unit;

        Vec2() {
            this->x = 0;
            this->y = 0;
        }

        Vec2(T _x, T _y) {
            this->x = _x;
            this->y = _y;
        }

        float magnitude() const {
            return static_cast<float>(std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
        }

        float magnitudeSquared() const {
            return static_cast<float>(std::pow(this->x, 2) + std::pow(this->y, 2));
        }

        bool isZero() const {
            return *this == Zero;
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
    const Vec2<T> Vec2<T>::Zero{0, 0};
    template<typename T>
    const Vec2<T> Vec2<T>::Unit{1, 1};

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vec2<T>& vector) {
        os << "(" << vector.x << ", " << vector.y << ")";
        return os;
    }
}
