#include "Physics.h"

namespace physics {
    PhysicsInfo::PhysicsInfo()
        : physicsEnabled(false),
          position(slurp::Vector2<int>::Zero),
          direction(slurp::Vector2<float>::Zero),
          speed(0) {}

    PhysicsInfo::PhysicsInfo(const slurp::Vector2<int>& position)
        : physicsEnabled(true),
          position(position),
          direction({}),
          speed(0) {}

    PhysicsInfo::PhysicsInfo(
        const slurp::Vector2<int>& position,
        float speed
    ): physicsEnabled(true),
       position(position),
       direction({}),
       speed(speed) {}

    slurp::Vector2<int> PhysicsInfo::getPositionUpdate(float dt) { return direction * speed * dt; }
}
