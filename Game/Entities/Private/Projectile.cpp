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
          isActive(false) {}

    void Projectile::fire(const slurp::Vector2<int>& position, const slurp::Vector2<float>& direction) {
        this->isActive = false;
        this->enabled = true;
        this->physicsInfo.position = position;
        this->physicsInfo.direction = direction;

        timer::delay(
            ActivationDelay,
            [this] {
                isActive = true;
            }
        );

        GlobalGameState->projectileIdx++;
        if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) { GlobalGameState->projectileIdx = 0; }
    }

    void Projectile::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (!isActive) { return; }

        if (Player* player = dynamic_cast<Player*>(collisionDetails.entity)) {
            if (player->isParryActive) {
                onParried();
            } else {
                this->enabled = false;
            }
        } else if (Enemy* enemy = dynamic_cast<Enemy*>(collisionDetails.entity)) {
            this->enabled = false;
        } else {
            this->physicsInfo.direction *= -1;
        }
    }

    void Projectile::onParried() {
        this->isParried = true;
        this->renderInfo.sprite = ProjectileParriedSprite;
        this->physicsInfo.speed = ParriedProjectileSpeed;
        this->physicsInfo.direction *= -1;
        // TODO: need to get timer_handle back and have a way to reset the timer if it's already active
        // TODO: need a way to specify timer_handle
        timer::delay(
            ParriedDuration,
            [this] {
                this->renderInfo.sprite = ProjectileSprite;
                this->physicsInfo.speed = BaseProjectileSpeed;
                this->isParried = false;
            }
        );
    }
}
