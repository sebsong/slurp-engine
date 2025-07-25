#include "Projectile.h"

#include "Game.h"

namespace game {
    static constexpr int ProjectileSizePixels = 10;
    static constexpr render::ColorPaletteIdx ProjectileColorPalletIdx = 5;
    static constexpr int BaseProjectileSpeed = 350;
    static constexpr int ParriedProjectileSpeed = 500;
    static const geometry::Shape projectileShape = {
        geometry::Rect,
        {ProjectileSizePixels, ProjectileSizePixels}
    };
    static constexpr float ActivationDelay = .2f;
    static constexpr float ParriedDuration = 2.f;
    static constexpr const char* ProjectileSpriteFileName = "projectile.bmp";
    static constexpr const char* ProjectileParriedSpriteFileName = "projectile_parried.bmp";
    static const render::Sprite ProjectileSprite = render::loadSprite(ProjectileSpriteFileName);
    static const render::Sprite ProjectileParriedSprite = render::loadSprite(ProjectileParriedSpriteFileName);

    Projectile::Projectile(int index)
        : Entity(
              "Projectile" + std::to_string(index),
              render::RenderInfo(
                  ProjectileSprite,
                  true
              ),
              physics::PhysicsInfo(
                  slurp::Vector2<int>::Zero,
                  BaseProjectileSpeed
              ),
              collision::CollisionInfo(
                  false,
                  true,
                  projectileShape,
                  true
              )
          ),
          _isActive(false) {}

    void Projectile::fire(const slurp::Vector2<int>& position) {
        this->_isActive = false;
        this->enabled = true;
        this->physicsInfo.position = position;
        this->_target = GlobalGameState->mouseCursor.getClosestEnemy();

        timer::delay(
            ActivationDelay,
            [this] {
                _isActive = true;
            }
        );

        GlobalGameState->projectileIdx++;
        if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) { GlobalGameState->projectileIdx = 0; }
    }

    void Projectile::update(float dt) {
        Entity::update(dt);

        if (_target) {
            this->physicsInfo.direction = static_cast<slurp::Vector2<float>>(
                this->_target->physicsInfo.position - this->physicsInfo.position
            ).normalize();
        }
    }

    void Projectile::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (!_isActive) { return; }

        if (Player* player = dynamic_cast<Player*>(collisionDetails.entity)) {
            if (player->isParryActive) {
                onParried();
            } else {
                this->enabled = false;
            }
        } else if (dynamic_cast<Enemy*>(collisionDetails.entity)) {
            this->_target = &GlobalGameState->player;
        } else {
            bounce();
        }
    }

    void Projectile::onParried() {
        this->_isParried = true;
        this->renderInfo.sprite = ProjectileParriedSprite;
        this->physicsInfo.speed = ParriedProjectileSpeed;
        this->_target = GlobalGameState->mouseCursor.getClosestEnemy();
        // TODO: need to get timer_handle back and have a way to reset the timer if it's already active
        // TODO: need a way to specify timer_handle
        timer::delay(
            ParriedDuration,
            [this] {
                this->renderInfo.sprite = ProjectileSprite;
                this->physicsInfo.speed = BaseProjectileSpeed;
                this->_isParried = false;
            }
        );
    }

    void Projectile::bounce() {
        // TODO: have a better bounce that actually reflects correctly
        this->physicsInfo.direction *= -1;
    }
}
