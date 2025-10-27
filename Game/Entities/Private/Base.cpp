#include "Base.h"

namespace base {
    static const geometry::Shape BaseShape = {geometry::Rect, {24, 5}};
    static const slurp::Vec2<float> RenderOffset = {0, 3};

    Base::Base(): Entity(
        "Base",
        render::RenderInfo(slurp::Globals->GameAssets->baseSprite, true, 0, RenderOffset),
        physics::PhysicsInfo(),
        collision::CollisionInfo(
            true,
            false,
            BaseShape,
            true
        )
    ) {}

    void Base::initialize() {
        Entity::initialize();
    }
}
