#include "Turret.h"

namespace turret {
    static constexpr float Range = 32.f;
    static constexpr float ShootCooldown = 1.f;
    static const slurp::Vec2<float> RenderOffset = {0, 5};

    static constexpr float OrbMaxHeightOffset = 0.f;
    static constexpr float OrbMinHeightOffset = -3.f;

    static constexpr float TurretSpawnTime = 1.f;
    static constexpr float TurretIdleAnimDuration = 2.f;

    enum Sprites: uint8_t {
        TurretOrb = 0,
        TurretPit = 1,
        TurretRangeIndicator = 2
    };

    Turret::Turret()
        : Entity(
              "Turret",
              render::RenderInfo(
                  (asset::SpriteInstance[3]){
                      asset::SpriteInstance(slurp::Globals->GameAssets->turretOrbSprite, RenderOffset),
                      asset::SpriteInstance(slurp::Globals->GameAssets->turretPitSprite, RenderOffset),
                      asset::SpriteInstance(
                          slurp::Globals->GameAssets->turretRangeIndicatorSprite,
                          game::BACKGROUND_ENTITY_Z,
                          RenderOffset
                      )
                  }
              ),
              physics::PhysicsInfo(),
              collision::CollisionInfo(
                  true,
                  true,
                  geometry::Shape{geometry::Rect, {2 * Range, 2 * Range}},
                  true
              )
          ),
          _finishedSpawn(false),
          _flipOrbPath(false),
          _target(nullptr),
          _currentShootCooldown(0),
          _orbMaxHeight(0),
          _orbMinHeight(0) {}

    void Turret::initialize() {
        Entity::initialize();
        _orbMaxHeight = renderInfo.sprites[TurretOrb].renderOffset.y + OrbMaxHeightOffset;
        _orbMinHeight = renderInfo.sprites[TurretOrb].renderOffset.y + OrbMinHeightOffset;
        renderInfo.sprites[TurretPit].renderingEnabled = false;
        renderInfo.sprites[TurretRangeIndicator].renderingEnabled = false;
        playAnimation(game::Assets->turretSpawnAnim, TurretSpawnTime);
        // TODO: add an onFinish callback to animations
        timer::delay(
            TurretSpawnTime,
            [this] {
                // playAnimation(game::Assets->turretIdleAnim, TurretIdleAnimDuration, true);
                _finishedSpawn = true;
                renderInfo.sprites[TurretPit].renderingEnabled = true;
                renderInfo.sprites[TurretRangeIndicator].renderingEnabled = true;
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
        playAnimation(game::Assets->turretShootAnim, ShootCooldown);
        _target->decrementCorruption();
        _currentShootCooldown = ShootCooldown;
    }

    void Turret::update(float dt) {
        Entity::update(dt);

        if (!_finishedSpawn) {
            return;
        }

        if (math::tween(
            renderInfo.sprites[TurretOrb].renderOffset.y,
            _orbMaxHeight,
            _orbMinHeight,
            TurretIdleAnimDuration,
            dt,
            _flipOrbPath
        )) {
            _flipOrbPath = !_flipOrbPath;
        }

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
