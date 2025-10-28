#include "MineSite.h"

namespace mine_site {
    static const geometry::Shape MineSiteShape = {geometry::Rect, {24, 12}};
    static const slurp::Vec2<float> RenderOffset = {0, 7};
    static const slurp::Vec2<float> StartPos = {-150, -75};
    static const slurp::Vec2<float> MiningLocationOffset = {12, 0};

    MineSite::MineSite(): Entity(
        "Mine Site",
        render::RenderInfo(slurp::Globals->GameAssets->mineSiteSprite, true, StartPos.y, RenderOffset),
        physics::PhysicsInfo(StartPos),
        collision::CollisionInfo(
            true,
            true,
            MineSiteShape,
            true
        )
    ) {}

    slurp::Vec2<float> MineSite::getMiningLocation() const {
        return physicsInfo.position + MiningLocationOffset;
    }

    void MineSite::update(float dt) {
        Entity::update(dt);
        // debug::drawPoint(getMiningLocation());
    }
}
