#pragma once

#define INT_MAX 2147483647
#include <cstdint>

namespace math
{
    inline uint8_t round(float num)
    {
        return static_cast<uint8_t>(num + 0.5f);
    }
    
    // Random float between [0, 1]
    inline float randomFloat()
    {
        return rand() / static_cast<float>(RAND_MAX);
    }

    inline float randomFloat(float min, float max)
    {
        return (randomFloat()) * (max - min) + min;
    }
}
