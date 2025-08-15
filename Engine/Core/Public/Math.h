#pragma once

namespace math {
    template<typename T>
        requires std::integral<T>
    T round(float num) {
        return static_cast<T>(num + 0.5f);
    }

    inline bool inRange(int n, int min, int max) {
        return min <= n && n <= max;
    }

    template<typename T>
    T getClamped(T n, T min, T max) {
        return std::max(min, std::min(n, max));
    }

    inline double getHypotenuse(float a, float b) {
        return std::sqrt(std::pow(a, 2) + std::pow(b, 2));
    }
}
