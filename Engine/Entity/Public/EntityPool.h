#pragma once
#include <cstdint>

namespace entity {
    struct Entity;

    template<typename T, uint32_t N>
    struct EntityPool {
        uint32_t nextIndex;
        T instances[N];

        EntityPool(const T& entity): nextIndex(0) {
            for (uint32_t i = 0; i < N; i++) {
                new(&instances[i]) T(entity);
                instances[i].enabled = false;
                entity::registerEntity(instances[i]);
            }
        }

        T& getNext() {
            if (nextIndex >= N) {
                nextIndex = 0;
            }
            return instances[nextIndex++];
        }
    };
}
