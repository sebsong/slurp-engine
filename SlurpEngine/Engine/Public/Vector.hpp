#pragma once

namespace slurp
{
    template <typename T>
    struct Vector2
    {
        T x;
        T y;
        
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

        Vector2 operator+(const Vector2& other)
        {
            return Vector2(this->x + other.x, this->y + other.y);
        }

        Vector2& operator+=(const Vector2& other)
        {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Vector2 operator-(const Vector2& other)
        {
            return Vector2(this->x - other.x, this->y - other.y);
        }

        template <typename TScalar>
        Vector2 operator*(const TScalar& scalar)
        {
            return Vector2(this->x * scalar, this->y * scalar);
        }

        template <typename TNew>
        operator Vector2<TNew>() const
        {
            return Vector2<TNew>(static_cast<TNew>(this->x), static_cast<TNew>(this->x));
        }
    };
}
