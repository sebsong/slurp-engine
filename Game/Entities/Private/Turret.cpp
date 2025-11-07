#include "Turret.h"

namespace turret {
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
    }

    void Turret::update(float dt) {
        Entity::update(dt);
    }
}
