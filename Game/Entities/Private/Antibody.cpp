#include "Antibody.h"

namespace antibody {
    static const geometry::Shape Shape = {geometry::Rect, {2, 2}};
    static constexpr float BaseSpeed = 300;
    static constexpr float BaseAcceleration = BaseSpeed * 16;

    Antibody::Antibody(): Entity(
        "Antibody",
        render::RenderInfo(
            slurp::Globals->GameAssets->antibodySprite,
            true,
            0,
            {0, 0}
        ),
        physics::PhysicsInfo(
            {},
            BaseSpeed,
            BaseAcceleration
        ),
        collision::CollisionInfo(
            false,
            true,
            Shape,
            true
        )
    ) {}

    void Antibody::initialize() {
        Entity::initialize();
        findTarget();
    }

    void Antibody::update(float dt) {
        Entity::update(dt);
        renderInfo.zOrder = physicsInfo.position.y;

        slurp::Vec2<float> targetLocation = _target->physicsInfo.position;
        if (worker::approxEqual(physicsInfo.position, targetLocation)) {
            physicsInfo.position = targetLocation;
            physicsInfo.speed = 0;
            physicsInfo.acceleration = 0;
            // if (!_isAtTarget) {
            //     _isAtTarget = true;
            // }
        } else {
            physicsInfo.direction = (targetLocation - physicsInfo.position).normalize();
            physicsInfo.acceleration = BaseAcceleration;
        }

    }

    void Antibody::findTarget() {
        // TODO: update this to only target corrupted workers
        if (worker::Worker* worker = game::State->workers.getRandomEnabledInstance()) {
            _target = worker;
        }
    }
}
