#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"
#include "Settings.h"

#include "GameGlobal.cpp"
#include "Obstacle.cpp"
#include "Base.cpp"
#include "MineSite.cpp"
#include "MineSiteSpawner.cpp"
#include "Worker.cpp"
#include "Antibody.cpp"
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
        Assets->mineSiteSprite = asset::loadSprite("gold.bmp");

        Assets->workerSprite = asset::loadSprite("worker.bmp");
        Assets->workerLoading0Sprite = asset::loadSprite("worker_loading_0.bmp");
        Assets->workerLoading1Sprite = asset::loadSprite("worker_loading_1.bmp");
        Assets->workerLoading2Sprite = asset::loadSprite("worker_loading_2.bmp");
        Assets->workerLoadedSprite = asset::loadSprite("worker_loaded.bmp");
        Assets->workerCorruptedSprite = asset::loadSprite("worker_corrupted.bmp");

        Assets->antibodySprite = asset::loadSprite("antibody.bmp");

        Assets->progressBarEmpty = asset::loadSprite("progress_bar_empty.bmp");
        Assets->progressBarFull = asset::loadSprite("progress_bar_full.bmp");

        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");

        // NOTE: https://opengameart.org/content/since-2-am
        Assets->backgroundMusic = asset::loadSound("since_2_am.wav");
        Assets->resourceCollected = asset::loadSound("resource_collected.wav");
        Assets->resourceCollectedLow = asset::loadSound("resource_collected_low.wav");
        Assets->errorCollect = asset::loadSound("error.wav");
        Assets->resourceDropOff = asset::loadSound("resource_drop_off.wav");
        Assets->spawnMineSite = asset::loadSound("spawn_mine_site.wav");
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
                render::RenderInfo(slurp::Globals->GameAssets->backgroundSprite, true, BACKGROUND_Z),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->border,
            entity::Entity(
                "Border",
                render::RenderInfo(slurp::Globals->GameAssets->borderSprite, true, BORDER_Z),
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

        registerEntity(
            State->base,
            base::Base()
        );

        registerEntity(
            State->progressBarEmpty,
            entity::Entity(
                "Progress Bar Empty",
                render::RenderInfo(slurp::Globals->GameAssets->progressBarEmpty, true, BORDER_Z),
                physics::PhysicsInfo({25, 20}),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->progressBarFull,
            entity::Entity(
                "Progress Bar Full",
                render::RenderInfo(slurp::Globals->GameAssets->progressBarFull, true, BORDER_Z),
                physics::PhysicsInfo({25, 20}),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->mineSiteSpawner,
            mine_site::MineSiteSpawner()
        );

        new(&State->mineSites) entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES>(mine_site::MineSite());
        new(&State->mineSpots) types::deque_arena<slurp::Vec2<float>>();

        new(&State->workers) entity::EntityPool<worker::Worker, MAX_NUM_WORKERS>(worker::Worker());
        new(&State->targetableCorruptedWorkers) types::deque_arena<worker::Worker*>();

        new(&State->antibodies) entity::EntityPool<antibody::Antibody, MAX_NUM_ANTIBODIES>(antibody::Antibody());

        registerEntity(
            State->mouseCursor,
            mouse_cursor::MouseCursor()
        );
    }

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target) {
        return entity->physicsInfo.position.distanceSquaredTo(target) < entity->physicsInfo.speed * 0.01f;
    }

}
