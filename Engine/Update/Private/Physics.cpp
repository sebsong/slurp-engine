#include "Physics.h"

#include "Math.h"

namespace physics {
    PhysicsInfo::PhysicsInfo()
        : physicsEnabled(false),
          position(slurp::Vector2<float>::Zero),
          direction(slurp::Vector2<float>::Zero),
          speed(0),
          acceleration(0),
          maxSpeed(0) {}

    PhysicsInfo::PhysicsInfo(const slurp::Vector2<float>& position)
        : physicsEnabled(true),
          position(position),
          direction({}),
          speed(0),
          acceleration(0),
          maxSpeed(0) {}

    PhysicsInfo::PhysicsInfo(
        const slurp::Vector2<float>& position,
        float speed,
        float acceleration
    ): physicsEnabled(true),
       position(position),
       direction({}),
       speed(0),
       acceleration(acceleration),
       maxSpeed(speed) {}

    void PhysicsInfo::updatePhysics(float dt) {
        speed += acceleration * dt;
        speed = math::getClamped(speed, 0.f, maxSpeed);
    }

    slurp::Vector2<float> PhysicsInfo::getPositionUpdate(float dt) const {
        return direction * speed * dt;
        // return direction * (0.5 * acceleration * std::pow(dt, 2) + _currentSpeed * dt);
    }
}
