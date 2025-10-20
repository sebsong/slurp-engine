#pragma once

#include <cstdlib>
#include <cstdint>

namespace slurp_random {
    inline void setRandomSeed(uint32_t seed) {
        srand(seed);
    }

    // Random float between [0, 1]
    inline float randomFloat() {
        return rand() / static_cast<float>(RAND_MAX);
    }

    inline float randomFloat(float min, float max) {
        return (randomFloat()) * (max - min) + min;
    }
}
