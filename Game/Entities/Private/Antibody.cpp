#include "Antibody.h"

#include "Random.h"

namespace antibody {
    static const geometry::Shape Shape = {geometry::Rect, {2, 2}};
    static constexpr float BaseSpeed = 125;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static constexpr float SpeedPenaltyMultiplier = 0.5f;

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
        if (_target) {
            renderInfo.zOrder = _target->renderInfo.zOrder - 1;
            slurp::Vec2<float> targetLocation = _target->physicsInfo.position;
            if (worker::almostAtTarget(this, targetLocation)) {
                physicsInfo.position = targetLocation;
                if (!_isAtTarget) {
                    physicsInfo.speed = 0;
                    physicsInfo.acceleration = 0;
                    _isAtTarget = true;
                    _target->physicsInfo.maxSpeed *= SpeedPenaltyMultiplier;
                }
            } else {
                physicsInfo.direction = (targetLocation - physicsInfo.position).normalize();
                physicsInfo.acceleration = BaseAcceleration;
            }
        } else {
            renderInfo.zOrder = physicsInfo.position.y;
        }
    }

    void Antibody::findTarget() {
        _target = random::pickRandom(game::State->activeCorruptedWorkers);
    }
}
