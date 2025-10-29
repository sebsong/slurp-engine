#pragma once

#include <cstdlib>
#include <cstdint>

namespace random {
    inline void setRandomSeed(uint32_t seed) {
        srand(seed);
    }

    inline bool randomBool() {
        return rand() % 2 == 0;
    }

    inline uint32_t randomIndex(uint32_t maxIndex) {
        return rand() % maxIndex;
    }

    // Random float between [0, 1]
    inline float randomFloat() {
        return rand() / static_cast<float>(RAND_MAX);
    }

    inline float randomFloat(float min, float max) {
        return (randomFloat()) * (max - min) + min;
    }
}
