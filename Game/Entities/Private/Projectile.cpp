#include "Projectile.h"

#include "Game.h"
#include "Timer.h"

namespace projectile {
    static constexpr float BaseSpeed = 350;
    static constexpr float ParriedSpeed = 500;
    static constexpr int ShapeSize = 10;
    static const geometry::Shape projectileShape = {
        geometry::Rect,
        {ShapeSize, ShapeSize}
    };
    static constexpr float ActivationDelay = .2f;
    static constexpr float ParriedDuration = 2.f;
    static constexpr const char* SpriteFileName = "projectile.bmp";
    static constexpr const char* ParriedSpriteFileName = "projectile_parried.bmp";
    static const render::Sprite Sprite = render::loadSprite(SpriteFileName);
    static const render::Sprite ParriedSprite = render::loadSprite(ParriedSpriteFileName);

    Projectile::Projectile(int index)
        : Entity(
              "Projectile" + std::to_string(index),
              render::RenderInfo(
                  Sprite,
                  true
              ),
              physics::PhysicsInfo(
                  slurp::Vector2<float>::Zero,
                  BaseSpeed
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

    void Projectile::fire(const slurp::Vector2<float>& position) {
        this->_isActive = false;
        this->enabled = true;
        this->physicsInfo.position = position;
        this->_target = game::GlobalGameState->mouseCursor.getClosestEnemy();

        timer::delay(
            ActivationDelay,
            [this] {
                _isActive = true;
            }
        );

        game::GlobalGameState->projectileIdx++;
        if (game::GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) { game::GlobalGameState->projectileIdx = 0; }
    }

    void Projectile::update(float dt) {
        Entity::update(dt);

        if (_target) {
            this->physicsInfo.direction = (this->_target->physicsInfo.position - this->physicsInfo.position).normalize();
        }
    }

    void Projectile::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (!_isActive) { return; }

        if (player::Player* player = dynamic_cast<player::Player*>(collisionDetails.entity)) {
            if (player->isParryActive) {
                onParried();
            } else {
                this->enabled = false;
            }
        } else if (dynamic_cast<enemy::Enemy*>(collisionDetails.entity)) {
            this->_target = &game::GlobalGameState->player;
        } else {
            bounce();
        }
    }

    void Projectile::onParried() {
        this->_isParried = true;
        this->renderInfo.sprite = ParriedSprite;
        this->physicsInfo.speed = ParriedSpeed;
        this->_target = game::GlobalGameState->mouseCursor.getClosestEnemy();
        timer::start(
            this->_parriedTimerHandle,
            ParriedDuration,
            false,
            [this] {
                this->renderInfo.sprite = Sprite;
                this->physicsInfo.speed = BaseSpeed;
                this->_isParried = false;
            }
        );
    }

    void Projectile::bounce() {
        // TODO: have a better bounce that actually reflects correctly
        this->physicsInfo.direction *= -1;
    }
}
