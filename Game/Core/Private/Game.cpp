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
        Assets->borderSprite = asset::loadSprite("border.bmp");
        Assets->enemySprite = asset::loadSprite("enemy.bmp");
        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");
        Assets->playerSprite = asset::loadSprite("player.bmp");
        Assets->playerParrySprite = asset::loadSprite("player_parry.bmp");
        Assets->projectileSprite = asset::loadSprite("projectile.bmp");
        Assets->projectileParriedSprite = asset::loadSprite(
            "projectile_parried.bmp"
        );

        Assets->backgroundMusic = asset::loadSound(
            global::BackgroundMusicSoundFileName
        );
        Assets->projectileHitSound = asset::loadSound(
            projectile::SoundFileName
        );
    }

    void initGame(bool isInitialized) {
        if (isInitialized) {
            Assets = slurp::Globals->GameAssets;
            State = slurp::Globals->GameState;
            return;
        }

        GameSystems* gameSystems = memory::permanent->allocate<GameSystems>();
        Assets = slurp::Globals->GameAssets = &gameSystems->assets;
        State = slurp::Globals->GameState = &gameSystems->state;

        loadAssets();

        State->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(State->randomSeed);

        slurp::Globals->RenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

        registerEntity(
            State->global,
            global::GameGlobal()
        );

        registerEntity(
            State->border,
            slurp::Entity(
                "Border",
                render::RenderInfo(slurp::Globals->GameAssets->borderSprite, true),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        // registerEntity(
        //     GlobalState->background,
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
            State->wallUp,
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, 20}};
        registerEntity(
            State->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, -CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            State->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {-CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            State->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        // geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        // registerEntity(
        //     GlobalState->obstacle1,
        //     obstacle::Obstacle(
        //         "Obstacle1",
        //         obstacle1Shape,
        //         {200, 500}
        //     )
        // );
        // geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        // registerEntity(
        //     GlobalState->obstacle2,
        //     obstacle::Obstacle(
        //         "Obstacle2",
        //         obstacle2Shape,
        //         {500, 400}
        //     )
        // );

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                State->projectiles[i],
                projectile::Projectile(i)
            );
        }

        registerEntity(
            State->player,
            player::Player()
        );

        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                State->enemies[i],
                enemy::Enemy(i)
            );
        }

        registerEntity(
            State->mouseCursor,
            mouse_cursor::MouseCursor()
        );
    }
}
