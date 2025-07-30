#pragma once
#include "Vector.h"

namespace physics {
    class PhysicsInfo {
    public:
        PhysicsInfo();

        explicit PhysicsInfo(const slurp::Vec2<float>& position);

        explicit PhysicsInfo(const slurp::Vec2<float>& position, float speed, float acceleration);

        void updatePhysics(float dt);

        slurp::Vec2<float> getPositionUpdate(float dt) const;

        bool physicsEnabled;
        slurp::Vec2<float> position;
        slurp::Vec2<float> direction;
        float speed;
        float acceleration;
        float maxSpeed;
    };
}
