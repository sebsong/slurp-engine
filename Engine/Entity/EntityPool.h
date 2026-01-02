#pragma once
#include <cstdint>

#include "Random.h"
#include "Scene.h"

namespace scene {
    struct Scene;
}

namespace entity {
    struct Entity;

    template<typename T, uint32_t N>
    class EntityPool {
    public:
        EntityPool() = default;

        EntityPool(scene::Scene* scene, const T& entity): _scene(scene) {
            for (T& instance: _instances) {
                // TODO: figure out if we can avoid registering all these upfront
                instance = entity;
                instance.enabled = false;
                _disabledInstances.push_back(&instance);
            }
        }

        T* nextInstance() {
            T* nextInstancePtr;
            if (!_disabledInstances.empty()) {
                nextInstancePtr = _disabledInstances.front();
                _disabledInstances.pop_front();
            } else {
                nextInstancePtr = _enabledInstances.front();
                _enabledInstances.pop_front();
            }
            return nextInstancePtr;
        }

        void enableInstance(T* instancePtr) {
            instancePtr->enable();
            _enabledInstances.push_back(instancePtr);
            scene::registerEntity(_scene, instancePtr);
            // TODO: make sure to remove from disabledInstances
        }

        T* newInstance(const slurp::Vec2<float>& position) {
            T* newInstancePtr = nextInstance();
            newInstancePtr->physicsInfo.position = position;
            enableInstance(newInstancePtr);
            return newInstancePtr;
        }

        T* getRandomEnabledInstance() {
            return rnd::pickRandom(_enabledInstances);
        }

        typename types::deque_arena<T*>::iterator begin() {
            return _enabledInstances.begin();
        }

        typename types::deque_arena<T*>::iterator end() {
            return _enabledInstances.end();
        }

        void recycleInstance(T* instancePtr) {
            for (auto it = _enabledInstances.begin(); it != _enabledInstances.end(); it++) {
                if (*it == instancePtr) {
                    _enabledInstances.erase(it);
                    break;
                }
            }
            instancePtr->enabled = false;
            _disabledInstances.push_back(instancePtr);
        }

        T* operator[](int index) {
            return &_instances[index];
        }

    private:
        T _instances[N];
        types::deque_arena<T*> _enabledInstances;
        types::deque_arena<T*> _disabledInstances;
        scene::Scene* _scene;
    };
}
