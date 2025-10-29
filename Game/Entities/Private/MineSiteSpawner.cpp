#include "MineSiteSpawner.h"

#include "Matrix.h"
#include "Random.h"

namespace mine_site {
    static constexpr float SpawnTime = 5.f;
    static const slurp::Mat22<float> OuterBounds = {
        {-CAMERA_WORLD_WIDTH_MAX + 15, CAMERA_WORLD_HEIGHT_MAX - 20},
        {CAMERA_WORLD_WIDTH_MAX - 15, -CAMERA_WORLD_HEIGHT_MAX + 10}
    };
    static const slurp::Mat22<float> InnerBounds = {
        {-75, 40},
        {75, -40}
    };

    MineSiteSpawner::MineSiteSpawner(): Entity("MineSiteSpawner") {}

    static slurp::Vec2<float> getRandomSpawnLocation() {
        float spawnX = random::randomFloat(OuterBounds.i.x, OuterBounds.j.x);

        float spawnY;
        if (math::inRange(spawnX, InnerBounds.i.x, InnerBounds.j.x)) {
            spawnY = random::randomBool()
                         ? random::randomFloat(OuterBounds.j.y, InnerBounds.j.y)
                         : random::randomFloat(InnerBounds.i.y, OuterBounds.i.y);
        } else {
            spawnY = random::randomFloat(OuterBounds.j.y, OuterBounds.i.y);
        }

        return {spawnX, spawnY};
    }

    static void SpawnMineSite() {
        MineSite* newMineSite = game::State->mineSites.newInstance();
        newMineSite->physicsInfo.position = getRandomSpawnLocation();
    }

    void MineSiteSpawner::initialize() {
        Entity::initialize();
        SpawnMineSite();
        timer::start(
            SpawnTime,
            true,
            [this] { SpawnMineSite(); }
        );
    }

    void MineSiteSpawner::update(float dt) {
        Entity::update(dt);
        // debug::drawRectBorder(
        //     OuterBounds.i,
        //     OuterBounds.j
        // );
        // debug::drawRectBorder(
        //     InnerBounds.i,
        //     InnerBounds.j
        // );
    }
}
