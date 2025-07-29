#include "Physics.h"

#include "Math.h"

namespace physics {
    PhysicsInfo::PhysicsInfo()
        : physicsEnabled(false),
          position(slurp::Vector2<float>::Zero),
          direction(slurp::Vector2<float>::Zero),
          maxSpeed(0),
          acceleration(0),
          _currentSpeed(0) {}

    PhysicsInfo::PhysicsInfo(const slurp::Vector2<float>& position)
        : physicsEnabled(true),
          position(position),
          direction({}),
          maxSpeed(0),
          acceleration(0),
          _currentSpeed(0) {}

    PhysicsInfo::PhysicsInfo(
        const slurp::Vector2<float>& position,
        float speed,
        float acceleration
    ): physicsEnabled(true),
       position(position),
       direction({}),
       maxSpeed(speed),
       acceleration(acceleration),
       _currentSpeed(0) {}

    void PhysicsInfo::updatePhysics(float dt) {
        _currentSpeed += acceleration * dt;
        _currentSpeed = math::getClamped(_currentSpeed, 0.f, maxSpeed);
    }

    slurp::Vector2<float> PhysicsInfo::getPositionUpdate(float dt) const {
        return direction * (0.5 * acceleration * std::pow(dt, 2) + _currentSpeed * dt);
    }
}
