#include "MineSite.h"

namespace mine_site {
    static const geometry::Shape MineSiteShape = {geometry::Rect, {24, 12}};
    static const slurp::Vec2<float> RenderOffset = {0, 7};
    static const slurp::Vec2<float> StartPos = {-150, -75};

    static constexpr uint32_t NumMiningLocations = 7;
    static const slurp::Vec2<float> MiningLocationOffsets[NumMiningLocations] = {
        {13, 0},
        {10, -4},
        {5, -7},
        {0, -8},
        {-5, -7},
        {-10, -4},
        {-13, 0},
    };

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
        return physicsInfo.position + MiningLocationOffsets[random::randomIndex(NumMiningLocations)];
    }

    void MineSite::update(float dt) {
        Entity::update(dt);
        renderInfo.zOrder = physicsInfo.position.y;
        // for (auto offset: MiningLocationOffsets) {
        //     debug::drawPoint(physicsInfo.position + offset);
        // }
    }
}
