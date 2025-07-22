#include "Projectile.h"

#include "Game.h"

namespace game {
    static constexpr int ProjectileSizePixels = 15;
    static constexpr render::ColorPaletteIdx ProjectileColorPalletIdx = 1;
    static constexpr int BaseProjectileSpeed = 500;
    static const geometry::Shape projectileShape = {
        geometry::Rect,
        {ProjectileSizePixels, ProjectileSizePixels}
    };
    static constexpr float ActivationDelay = .5f;

    Projectile::Projectile(int index)
        : Entity(
              "Projectile" + std::to_string(index),
              render::RenderInfo(
                  render::RenderShape{
                      projectileShape,
                      getColor(ProjectileColorPalletIdx)
                  },
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
                std::cout << "ACTIVATE" << std::endl;
                isActive = true;
            }
        );

        GlobalGameState->projectileIdx++;
        if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) { GlobalGameState->projectileIdx = 0; }
    }

    void Projectile::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (!isActive) { return; }

        this->physicsInfo.direction *= -1;
    }
}
