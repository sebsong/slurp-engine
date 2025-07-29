#include "Physics.h"

namespace physics {
    PhysicsInfo::PhysicsInfo()
        : physicsEnabled(false),
          position(slurp::Vector2<float>::Zero),
          direction(slurp::Vector2<float>::Zero),
          speed(0) {}

    PhysicsInfo::PhysicsInfo(const slurp::Vector2<float>& position)
        : physicsEnabled(true),
          position(position),
          direction({}),
          speed(0) {}

    PhysicsInfo::PhysicsInfo(
        const slurp::Vector2<float>& position,
        float speed
    ): physicsEnabled(true),
       position(position),
       direction({}),
       speed(speed) {}

    slurp::Vector2<float> PhysicsInfo::getPositionUpdate(float dt) {
        return direction * speed * dt;
    }
}
