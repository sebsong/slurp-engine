#pragma once

#define INT_MAX 2147483647

namespace slurp
{
    inline uint8_t round(float num)
    {
        return static_cast<uint8_t>(num + 0.5f);
    }
}
