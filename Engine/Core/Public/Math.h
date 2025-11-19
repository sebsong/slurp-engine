#pragma once

namespace math {
    template<typename T>
        requires std::integral<T>
    T round(float num) {
        return static_cast<T>(num + 0.5f);
    }

    template<typename T>
    bool inRange(T n, T min, T max) {
        return min <= n && n <= max;
    }

    template<typename T>
    T getClamped(T n, T min, T max) {
        return std::max(min, std::min(n, max));
    }

    template<typename T>
    slurp::Vec2<T> getClamped(slurp::Vec2<T> n, slurp::Vec2<T> min, slurp::Vec2<T> max) {
        return {
            getClamped(n.x, min.x, max.x),
            getClamped(n.y, min.y, max.y)
        };
    }

    inline double getHypotenuse(float a, float b) {
        return std::sqrt(std::pow(a, 2) + std::pow(b, 2));
    }

    /** Returns true when val has reached `to` **/
    inline bool tween(float& val, float from, float to, float t, float dt, bool flip = false) {
        if (flip) {
            std::swap(from, to);
        }

        float delta = (to - from) * (dt / t); // TODO: make this not linear
        float newVal = val + delta;

        if (to < from) {
            val = std::max(newVal, to);
        } else {
            val = std::min(newVal, to);
        }

        return val == to;
    }
}
