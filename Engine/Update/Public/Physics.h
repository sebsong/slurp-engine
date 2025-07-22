#pragma once
#include "Vector.h"

namespace physics {
    struct PhysicsInfo {
        bool physicsEnabled;
        slurp::Vector2<int> position; // TODO: should make this a float vector
        slurp::Vector2<float> direction;
        float speed;
        // TODO: acceleration

        PhysicsInfo();

        PhysicsInfo(const slurp::Vector2<int>& position);

        PhysicsInfo(const slurp::Vector2<int>& position, float speed);

        slurp::Vector2<int> getPositionUpdate(float dt);
    };
}
