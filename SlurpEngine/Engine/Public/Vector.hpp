#pragma once
#include <iostream>
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

        template <typename U>
        Vector2<std::common_type_t<T, U>> operator+(const Vector2<U>& other) const
        {
            return Vector2<std::common_type_t<T, U>>(this->x + other.x, this->y + other.y);
        }
        
        Vector2& operator+=(const Vector2& other)
        {
            this->x += other.x;
            this->y += other.y;
            return *this;
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

        template <typename TNew>
        operator Vector2<TNew>() const
        {
            return Vector2<TNew>(static_cast<TNew>(this->x), static_cast<TNew>(this->y));
        }
    };

    template <typename T>
    const Vector2<T> Vector2<T>::Zero{1, 1};
    template <typename T>
    const Vector2<T> Vector2<T>::Unit{1, 1};
    
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& vector)
    {
        os << "(" << vector.x << ", " << vector.y << ")";
        return os;
    }
}
