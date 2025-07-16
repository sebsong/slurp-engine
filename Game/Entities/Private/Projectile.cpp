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

    Projectile::Projectile(int index): Entity(
        "Projectile" + std::to_string(index),
        projectileShape,
        true,
        getColor(ProjectileColorPalletIdx),
        slurp::Vector2<int>::Zero,
        BaseProjectileSpeed,
        collision::CollisionInfo(
            false,
            projectileShape,
            true
        )
    ) {}

    void Projectile::onCollisionEnter(const Entity* other) {
        Entity::onCollisionEnter(other);

        this->direction *= -1;
    }
}
