#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"

#include "GameGlobal.cpp"
#include "Obstacle.cpp"
#include "Player.cpp"
#include "Enemy.cpp"
#include "Projectile.cpp"
#include "MouseCursor.cpp"
#include "Settings.h"

namespace game {
    template<typename T>
    static void registerEntity(
        T& entityLocation,
        T&& entity
    ) {
        // TODO: this pattern is a little weird, also need to know to include new properties in the move constructor
        new(&entityLocation) T(std::forward<T>(entity));
        slurp::registerEntity(entityLocation);
    }

    static void loadAssets() {
        slurp::Globals->GameAssets->borderSprite = asset::loadSprite("border.bmp");
        slurp::Globals->GameAssets->enemySprite = asset::loadSprite("enemy.bmp");
        slurp::Globals->GameAssets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");
        slurp::Globals->GameAssets->playerSprite = asset::loadSprite("player.bmp");
        slurp::Globals->GameAssets->playerParrySprite = asset::loadSprite("player_parry.bmp");
        slurp::Globals->GameAssets->projectileSprite = asset::loadSprite("projectile.bmp");
        slurp::Globals->GameAssets->projectileParriedSprite = asset::loadSprite(
            "projectile_parried.bmp"
        );

        slurp::Globals->GameAssets->backgroundMusic = asset::loadSound(
            global::BackgroundMusicSoundFileName
        );
        slurp::Globals->GameAssets->projectileHitSound = asset::loadSound(
            projectile::SoundFileName
        );
    }

    void initGame(bool isInitialized) {
        if (isInitialized) {
            return;
        }

        GameSystems* gameSystems = slurp::Globals->GameMemory->permanent->allocate<GameSystems>();
        slurp::Globals->GameAssets = &gameSystems->assets;
        slurp::Globals->GameState = &gameSystems->state;

        loadAssets();

        slurp::Globals->GameState->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(slurp::Globals->GameState->randomSeed);

        slurp::Globals->RenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

        registerEntity(
            slurp::Globals->GameState->global,
            global::GameGlobal()
        );

        registerEntity(
            slurp::Globals->GameState->border,
            slurp::Entity(
                "Border",
                render::RenderInfo(slurp::Globals->GameAssets->borderSprite, true),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        // registerEntity(
        //     slurp::Globals->GameState->background,
        //     slurp::Entity(
        //         "Background",
        //         render::RenderInfo(
        //             render::RenderShape{
        //                 {geometry::Rect, {1280, 720}},
        //                 getColor(2)
        //             },
        //             false
        //         ),
        //         physics::PhysicsInfo(),
        //         collision::CollisionInfo()
        //     )
        // );
        geometry::Shape wallUpShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, 20}};
        registerEntity(
            slurp::Globals->GameState->wallUp,
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, 20}};
        registerEntity(
            slurp::Globals->GameState->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, -CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            slurp::Globals->GameState->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {-CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            slurp::Globals->GameState->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        // geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        // registerEntity(
        //     slurp::Globals->GameState->obstacle1,
        //     obstacle::Obstacle(
        //         "Obstacle1",
        //         obstacle1Shape,
        //         {200, 500}
        //     )
        // );
        // geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        // registerEntity(
        //     slurp::Globals->GameState->obstacle2,
        //     obstacle::Obstacle(
        //         "Obstacle2",
        //         obstacle2Shape,
        //         {500, 400}
        //     )
        // );

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                slurp::Globals->GameState->projectiles[i],
                projectile::Projectile(i)
            );
        }

        registerEntity(
            slurp::Globals->GameState->player,
            player::Player()
        );

        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                slurp::Globals->GameState->enemies[i],
                enemy::Enemy(i)
            );
        }

        registerEntity(
            slurp::Globals->GameState->mouseCursor,
            mouse_cursor::MouseCursor()
        );
    }
}
