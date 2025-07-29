#pragma once
#include "Vector.h"

namespace physics {
    class PhysicsInfo {
    public:
        PhysicsInfo();

        PhysicsInfo(const slurp::Vector2<float>& position);

        PhysicsInfo(const slurp::Vector2<float>& position, float speed, float acceleration);

        void updatePhysics(float dt);

        slurp::Vector2<float> getPositionUpdate(float dt) const;

        bool physicsEnabled;
        slurp::Vector2<float> position;
        slurp::Vector2<float> direction;
        float maxSpeed;
        float acceleration;

    private:
        float _currentSpeed;
    };
}
