#pragma once
#include <cstdint>

#include "Random.h"

namespace entity {
    struct Entity;

    template<typename T, uint32_t N>
    class EntityPool {
    public:
        EntityPool(const T& entity) {
            for (uint32_t i = 0; i < N; i++) {
                T* instancePtr = &instances[i];
                new(instancePtr) T(entity);
                instancePtr->enabled = false;
                disabledInstances.push_back(instancePtr);
            }
        }

        T* newInstance() {
            T* newInstancePtr;
            if (!disabledInstances.empty()) {
                newInstancePtr = disabledInstances.front();
                disabledInstances.pop_front();
                entity::registerEntity(*newInstancePtr);
            } else {
                newInstancePtr = enabledInstances.front();
                enabledInstances.pop_front();
            }
            newInstancePtr->enable();
            enabledInstances.push_back(newInstancePtr);
            return newInstancePtr;
        }

        T* getRandomEnabledInstance() {
            uint32_t numEnabledInstances = enabledInstances.size();
            if (numEnabledInstances == 0) {
                return nullptr;
            }
            uint32_t randomIndex = random::randomIndex(numEnabledInstances);
            return enabledInstances.at(randomIndex);
        }

        void recycleInstance(T* instancePtr) {
            for (auto it = enabledInstances.begin(); it != enabledInstances.end(); it++) {
                if (*it == instancePtr) {
                    enabledInstances.erase(it);
                    break;
                }
            }
            instancePtr->enabled = false;
            disabledInstances.push_back(instancePtr);
        }

    private:
        T instances[N];
        types::deque_arena<T*> enabledInstances;
        types::deque_arena<T*> disabledInstances;
    };
}
