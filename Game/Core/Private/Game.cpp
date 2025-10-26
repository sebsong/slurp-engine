#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"
#include "Settings.h"

#include "GameGlobal.cpp"
#include "Obstacle.cpp"
#include "Worker.cpp"
#include "Player.cpp"
#include "Enemy.cpp"
#include "Projectile.cpp"
#include "MouseCursor.cpp"

namespace game {
    template<typename T>
    static void registerEntity(
        T& entityLocation,
        T&& entity
    ) {
        // TODO: this pattern is a little weird, also need to know to include new properties in the move constructor
        if (entityLocation.id != entity::INVALID_ENTITY_ID) {
            // Entity is already initialized, this is a hot reload
            // re-instantiate to re-initialize vtable
            new(&entityLocation) T(entityLocation);
        } else {
            new(&entityLocation) T(std::forward<T>(entity));
            entity::registerEntity(entityLocation);
        }
    }

    static void loadAssets() {
        Assets->backgroundSprite = asset::loadSprite("background.bmp");
        Assets->borderSprite = asset::loadSprite("border.bmp");
        Assets->baseSprite = asset::loadSprite("base.bmp");
        Assets->workerSprite = asset::loadSprite("worker.bmp");
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
        } else {
            GameSystems* gameSystems = memory::Permanent->allocate<GameSystems>();
            Assets = slurp::Globals->GameAssets = &gameSystems->assets;
            State = slurp::Globals->GameState = &gameSystems->state;
            loadAssets();
        }

        State->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(State->randomSeed);

        slurp::Globals->RenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

        registerEntity(
            State->global,
            global::GameGlobal()
        );

        registerEntity(
            State->background,
            entity::Entity(
                "Background",
                render::RenderInfo(slurp::Globals->GameAssets->backgroundSprite, true),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->border,
            entity::Entity(
                "Border",
                render::RenderInfo(slurp::Globals->GameAssets->borderSprite, true),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        float wallThickness = 10;
        geometry::Shape wallUpShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, wallThickness}};
        registerEntity(
            State->wallUp,
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, wallThickness}};
        registerEntity(
            State->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, -CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {wallThickness, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            State->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {-CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {wallThickness, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            State->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );

        geometry::Shape baseShape = {geometry::Rect, {24, 10}};
        registerEntity(
            State->base,
            entity::Entity(
                "Base",
                render::RenderInfo(slurp::Globals->GameAssets->baseSprite, true, {0, 7}),
                physics::PhysicsInfo(),
                collision::CollisionInfo(
                    true,
                    false,
                    baseShape,
                    true
                )
            )
        );

        registerEntity(
            State->worker,
            worker::Worker()
        );

        registerEntity(
            State->mouseCursor,
            mouse_cursor::MouseCursor()
        );
    }
}
