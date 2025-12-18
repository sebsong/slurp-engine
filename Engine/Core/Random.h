#pragma once

#include <cstdlib>
#include <cstdint>
#include <random>

namespace rnd {
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

    template<typename C, typename T>
    T pickRandom(const C& collection, const T defaultValue) {
        if (collection.empty()) {
            return defaultValue;
        }

        return collection.at(randomIndex(collection.size()));
    }

    template<typename C, typename T>
    T* pickRandom(const C& collection) {
        return pickRandom<C, T*>(collection, nullptr);
    }

    template<typename C>
    void shuffle(C& collection) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(collection.begin(), collection.end(), g);
    }
}
