#include "MineSite.h"

namespace mine_site {
    static const geometry::Shape MineSiteShape = {geometry::Rect, {24, 12}};
    static const slurp::Vec2<float> RenderOffset = {0, 9};
    static const slurp::Vec2<float> StartPos = {-150, -75};

    static constexpr uint32_t NumMiningLocations = 7;
    static const slurp::Vec2<float> MineSpotOffsets[NumMiningLocations] = {
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
        render::RenderInfo(slurp::Globals->GameAssets->mineSiteSprite, true, true, RenderOffset),
        physics::PhysicsInfo(StartPos),
        collision::CollisionInfo()
    ) {}

    void MineSite::initialize() {
        Entity::initialize();
        for (slurp::Vec2 offset: MineSpotOffsets) {
            game::State->mineSpots.push_back(physicsInfo.position + offset);
        }
    }

    void MineSite::update(float dt) {
        Entity::update(dt);
        // for (auto offset: MineSpotOffsets) {
        //     debug::drawPoint(physicsInfo.position + offset);
        // }
        // debug::drawPoint(physicsInfo.position);
    }
}
