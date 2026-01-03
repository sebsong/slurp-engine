#include "MineSiteSpawner.h"

#include "Game.h"
#include "Matrix.h"
#include "MineSite.h"
#include "Random.h"
#include "Settings.h"

namespace mine_site {
    static const slurp::Mat22<float> OuterBounds = {
        {-WORLD_WIDTH_MAX + 20, WORLD_HEIGHT_MAX - 55},
        {WORLD_WIDTH_MAX - 20, -WORLD_HEIGHT_MAX + 15}
    };
    static const slurp::Mat22<float> InnerBounds = {
        {-75, 40},
        {75, -40}
    };

    MineSiteSpawner::MineSiteSpawner(): Entity("MineSiteSpawner") {}

    static slurp::Vec2<float> getRandomSpawnLocation() {
        float spawnX = rnd::randomFloat(OuterBounds.i.x, OuterBounds.j.x);

        float spawnY;
        if (math::inRange(spawnX, InnerBounds.i.x, InnerBounds.j.x)) {
            spawnY = rnd::randomBool()
                         ? rnd::randomFloat(OuterBounds.j.y, InnerBounds.j.y)
                         : rnd::randomFloat(InnerBounds.i.y, OuterBounds.i.y);
        } else {
            spawnY = rnd::randomFloat(OuterBounds.j.y, OuterBounds.i.y);
        }

        return {spawnX, spawnY};
    }

    void MineSiteSpawner::spawnMineSite() {
        MineSite* newMineSite = game::GameScene->mineSites.nextInstance();
        newMineSite->physicsInfo.position = getRandomSpawnLocation();
        game::GameScene->mineSites.enableInstance(newMineSite);
        newMineSite->playAnimation(game::Assets->mineSiteSpawnAnim, 2.f);
        audio::play(game::Assets->spawnMineSite, 0.2f);
    }

    void MineSiteSpawner::initialize() {
        Entity::initialize();
        spawnMineSite();
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
