#pragma once
#include <cstdint>

#include "Random.h"

namespace entity {
    struct Entity;

    template<typename T, uint32_t N>
    class EntityPool {
    public:
        EntityPool() = default;

        EntityPool(const T& entity) {
            for (T& instance: instances) {
                // TODO: figure out if we can avoid registering all these upfront
                instance = entity;
                instance.enabled = false;
                disabledInstances.push_back(&instance);
            }
        }

        T* nextInstance() {
            T* nextInstancePtr;
            if (!disabledInstances.empty()) {
                nextInstancePtr = disabledInstances.front();
                disabledInstances.pop_front();
            } else {
                nextInstancePtr = enabledInstances.front();
                enabledInstances.pop_front();
            }
            return nextInstancePtr;
        }

        void enableInstance(T* instancePtr) {
            instancePtr->enable();
            enabledInstances.push_back(instancePtr);
            // TODO: make sure to remove from disabledInstances
        }

        T* newInstance(const slurp::Vec2<float>& position) {
            T* newInstancePtr = nextInstance();
            newInstancePtr->physicsInfo.position = position;
            enableInstance(newInstancePtr);
            return newInstancePtr;
        }

        T* getRandomEnabledInstance() {
            return rnd::pickRandom(enabledInstances);
        }

        typename types::deque_arena<T*>::iterator begin() {
            return enabledInstances.begin();
        }

        typename types::deque_arena<T*>::iterator end() {
            return enabledInstances.end();
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

        T* operator[](int index) {
            return &instances[index];
        }

    private:
        T instances[N];
        types::deque_arena<T*> enabledInstances;
        types::deque_arena<T*> disabledInstances;
    };
}
