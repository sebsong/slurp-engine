#include "Turret.h"

namespace turret {
    static constexpr float Range = 32.f;
    static constexpr float ShootCooldown = .5f;

    Turret::Turret(): Entity(
        "Turret",
        render::RenderInfo(
            slurp::Globals->GameAssets->turretSprite,
            true,
            0,
            {0, 0}
        ),
        physics::PhysicsInfo(),
        collision::CollisionInfo()
    ) {}

    void Turret::initialize() {
        Entity::initialize();
        game::State->turretsRangeIndicators.newInstance()->physicsInfo.position = physicsInfo.position;
    }

    worker::Worker* Turret::findCorruptedWorkerInRange(
        types::deque_arena<worker::Worker*> corruptedWorkers,
        float range
    ) {
        worker::Worker* targetWorker = nullptr;
        float closestDistance = std::numeric_limits<float>::max();
        for (worker::Worker* worker: corruptedWorkers) {
            float distance = physicsInfo.position.distanceTo(worker->physicsInfo.position);
            if (distance <= range && (targetWorker == nullptr || distance < closestDistance)) {
                targetWorker = worker;
            }
        }

        return targetWorker;
    }

    void Turret::shootAtTarget() {
        logging::debug("FIRE AT:");
        logging::debug(_target->physicsInfo.position);
        audio::play(game::Assets->turretShoot);
        _target->purify();
        _currentShootCooldown = ShootCooldown;
    }

    void Turret::update(float dt) {
        Entity::update(dt);
        renderInfo.zOrder = physicsInfo.position.y;

        _target = findCorruptedWorkerInRange(game::State->targetableCorruptedWorkers, Range);

        _currentShootCooldown -= dt;
        if (_target && _currentShootCooldown <= 0) {
            shootAtTarget();
        }

        // debug::drawRectBorder(
        //     physicsInfo.position + slurp::Vec2{-Range, -Range},
        //     physicsInfo.position + slurp::Vec2{Range, Range}
        // );
    }
}
