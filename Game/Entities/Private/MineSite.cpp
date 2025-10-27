#include "MineSite.h"

namespace mine_site {
    static const geometry::Shape GoldShape = {geometry::Rect, {16, 7}};
    static const slurp::Vec2<float> RenderOffset = {0, 7};
    static const slurp::Vec2<float> StartPos = {-150, -75};

    MineSite::MineSite(): Entity(
        "Mine Site",
        render::RenderInfo(slurp::Globals->GameAssets->goldSprite, true, StartPos.y, RenderOffset),
        physics::PhysicsInfo(StartPos),
        collision::CollisionInfo(
            true,
            false,
            GoldShape,
            true
        )
    ) {}
}
