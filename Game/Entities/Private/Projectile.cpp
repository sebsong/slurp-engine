#include "Projectile.h"

#include "Game.h"
#include "Timer.h"

namespace projectile {
    static constexpr float BaseSpeed = 350;
    static constexpr float BaseAcceleration = BaseSpeed * 8;
    static constexpr float ParriedSpeed = 500;
    static constexpr int ShapeSize = 10;
    static const geometry::Shape projectileShape = {
        geometry::Rect,
        {ShapeSize, ShapeSize}
    };
    static constexpr float ActivationDelay = .2f;
    static constexpr float ParriedDuration = 2.f;

    static constexpr const char* SoundFileName = "hit.wav";

    Projectile::Projectile(int index)
        : Entity(
              "Projectile" + std::to_string(index),
              render::RenderInfo(
                  slurp::Globals->GameAssets->projectileSprite,
                  true
              ),
              physics::PhysicsInfo(
                  slurp::Vec2<float>::Zero,
                  BaseSpeed,
                  BaseAcceleration
              ),
              collision::CollisionInfo(
                  false,
                  true,
                  projectileShape,
                  true
              )
          ),
          _isActive(false),
          _isParried(false),
          _target(nullptr),
          _parriedTimerHandle(timer::getNewHandle()) {
        this->enabled = false;
    }

    void Projectile::fire(const slurp::Vec2<float>& position) {
        this->_isActive = false;
        this->enabled = true;
        this->physicsInfo.position = position;
        this->_target = slurp::Globals->GameState->mouseCursor.getClosestEnemy();

        timer::delay(
            ActivationDelay,
            [this] {
                _isActive = true;
            }
        );

        slurp::Globals->GameState->projectileIdx++;
        if (slurp::Globals->GameState->projectileIdx >= PROJECTILE_POOL_SIZE) { slurp::Globals->GameState->projectileIdx = 0; }
    }

    void Projectile::update(float dt) {
        Entity::update(dt);

        if (_target) {
            this->physicsInfo.direction = (this->_target->physicsInfo.position - this->physicsInfo.position).
                    normalize();
        }
    }

    void Projectile::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (!_isActive) { return; }

        audio::playSound(slurp::Globals->GameAssets->projectileHitSound, 0.5f, false);

        if (player::Player* player = dynamic_cast<player::Player*>(collisionDetails.entity)) {
            if (player->isParryActive) {
                onParried();
            } else {
                this->enabled = false;
            }
        } else if (dynamic_cast<enemy::Enemy*>(collisionDetails.entity)) {
            this->_target = &slurp::Globals->GameState->player;
        } else {
            bounce();
        }
    }

    void Projectile::onParried() {
        this->_isParried = true;
        this->renderInfo.sprite = slurp::Globals->GameAssets->projectileParriedSprite;
        this->physicsInfo.maxSpeed = ParriedSpeed;
        this->_target = slurp::Globals->GameState->mouseCursor.getClosestEnemy();
        timer::start(
            this->_parriedTimerHandle,
            ParriedDuration,
            [this] {
                this->renderInfo.sprite = slurp::Globals->GameAssets->projectileSprite;
                this->physicsInfo.maxSpeed = BaseSpeed;
                this->_isParried = false;
            }
        );
    }

    void Projectile::bounce() {
        // TODO: have a better bounce that actually reflects correctly
        this->physicsInfo.direction *= -1;
    }
}
