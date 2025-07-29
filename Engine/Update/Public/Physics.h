#pragma once
#include "Vector.h"

namespace physics {
    struct PhysicsInfo {
        bool physicsEnabled;
        slurp::Vector2<float> position;
        slurp::Vector2<float> direction;
        float speed;
        // TODO: acceleration

        PhysicsInfo();

        PhysicsInfo(const slurp::Vector2<float>& position);

        PhysicsInfo(const slurp::Vector2<float>& position, float speed);

        slurp::Vector2<float> getPositionUpdate(float dt);
    };
}
