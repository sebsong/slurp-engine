#pragma once
#include <ostream>

namespace slurp
{
    template <typename T>
    struct Vector2
    {
        T x;
        T y;

        static const Vector2 Zero;
        static const Vector2 Unit;

        Vector2()
        {
            this->x = 0;
            this->y = 0;
        }

        Vector2(T _x, T _y)
        {
            this->x = _x;
            this->y = _y;
        }

        float magnitude()
        {
            return static_cast<float>(std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
        }

        // TODO: this can't properly normalize an int vector. need to convert to float first
        Vector2<float> normalize()
        {
            float mag = magnitude();
            if (mag != 0.0f)
            {
                *this /= mag;
            }
            return *this;
        }

        template <typename U>
        bool operator==(const Vector2<U>& other) const
        {
            return this->x == other.x && this->y == other.y;
        }

        template <typename U>
        Vector2<std::common_type_t<T, U>> operator+(const Vector2<U>& other) const
        {
            return Vector2<std::common_type_t<T, U>>(this->x + other.x, this->y + other.y);
        }

        template <typename U>
        Vector2<std::common_type_t<T, U>> operator-(const Vector2<U>& other) const
        {
            return Vector2<std::common_type_t<T, U>>(this->x - other.x, this->y - other.y);
        }

        template <typename TScalar>
        Vector2<std::common_type_t<T, TScalar>> operator*(const TScalar& scalar) const
        {
            return Vector2<std::common_type_t<T, TScalar>>(this->x * scalar, this->y * scalar);
        }

        template <typename TScalar>
        Vector2<std::common_type_t<T, TScalar>> operator/(const TScalar& scalar) const
        {
            return Vector2<std::common_type_t<T, TScalar>>(this->x / scalar, this->y / scalar);
        }

        Vector2& operator+=(const Vector2& other)
        {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Vector2& operator-=(const Vector2& other)
        {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        }

        template <typename TScalar>
        Vector2& operator*=(const TScalar& scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            return *this;
        }

        template <typename TScalar>
        Vector2& operator/=(const TScalar& scalar)
        {
            this->x = static_cast<T>(this->x / scalar);
            this->y = static_cast<T>(this->y / scalar);
            return *this;
        }

        template <typename TNew>
        operator Vector2<TNew>() const
        {
            return Vector2<TNew>(static_cast<TNew>(this->x), static_cast<TNew>(this->y));
        }
    };

    template <typename T>
    const Vector2<T> Vector2<T>::Zero{0, 0};
    template <typename T>
    const Vector2<T> Vector2<T>::Unit{1, 1};

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& vector)
    {
        os << "(" << vector.x << ", " << vector.y << ")";
        return os;
    }
}
