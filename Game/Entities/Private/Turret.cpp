#include "Turret.h"

namespace turret {
    static constexpr float Range = 32.f;
    static constexpr float ShootCooldown = .75f;
    static const slurp::Vec2<float> RenderOffset = {0, 5};

    static constexpr float TurretSpawnTime = 1.f;
    static constexpr float TurretIdleAnimDuration = 4.f;

    Turret::Turret(): Entity(
        "Turret",
        render::RenderInfo(
            (asset::SpriteInstance[2]){
                asset::SpriteInstance(slurp::Globals->GameAssets->turretSprite, RenderOffset),
                asset::SpriteInstance(slurp::Globals->GameAssets->turretRangeIndicatorSprite, game::BACKGROUND_ENTITY_Z, RenderOffset)
            }
        ),
        physics::PhysicsInfo(),
        collision::CollisionInfo(
            true,
            true,
            geometry::Shape{geometry::Rect, {2 * Range, 2 * Range}},
            true
        )
    ) {}

    void Turret::initialize() {
        Entity::initialize();
        playAnimation(game::Assets->turretSpawnAnim, TurretSpawnTime);
        timer::delay(
            TurretSpawnTime,
            [this] {
                playAnimation(game::Assets->turretIdleAnim, TurretIdleAnimDuration, true);
            }
        );
    }

    worker::Worker* Turret::findClosestCorruptedWorkerInRange(
        types::set_arena<Entity*> potentialTargets,
        float range
    ) {
        worker::Worker* targetWorker = nullptr;
        float closestDistance = std::numeric_limits<float>::max();
        for (Entity* entity: potentialTargets) {
            if (worker::Worker* worker = dynamic_cast<worker::Worker*>(entity)) {
                if (worker->isCorrupted()) {
                    float distance = physicsInfo.position.distanceTo(worker->physicsInfo.position);
                    if (distance <= range && (targetWorker == nullptr || distance < closestDistance)) {
                        targetWorker = worker;
                    }
                }
            }
        }

        return targetWorker;
    }

    void Turret::shootAtTarget() {
        audio::play(game::Assets->turretShoot);
        _target->decrementCorruption();
        _currentShootCooldown = ShootCooldown;
    }

    void Turret::update(float dt) {
        Entity::update(dt);
        _target = findClosestCorruptedWorkerInRange(collisionInfo.collidingWith, Range);

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
