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
#include "Turret.cpp"
#include "MouseCursor.cpp"


namespace game {
    static void loadAssets() {
        Assets->backgroundSprite = asset::loadSprite("background.bmp");
        Assets->borderSprite = asset::loadSprite("border.bmp");

        Assets->baseSprite = asset::loadSprite("base.bmp");
        Assets->mineSiteSprite = asset::loadSprite("gold.bmp");
        Assets->mineSiteSpawnAnim = asset::loadSpriteAnimation("gold_spawn.bmp", 9);

        Assets->workerSprite = asset::loadSprite("worker.bmp");
        Assets->workerLoadingAnim = asset::loadSpriteAnimation("worker_loading.bmp", 6);
        Assets->workerLoadedSprite = asset::loadSprite("worker_loaded.bmp");
        Assets->workerCorruptedSprite = asset::loadSprite("worker_corrupted.bmp");

        Assets->turretSprite = asset::loadSprite("turret.bmp");
        Assets->turretRangeIndicatorSprite = asset::loadSprite("turret_range_indicator.bmp");
        Assets->turretRangeIndicatorSprite->material.alpha = 0.3f;
        Assets->turretShootAnimation = asset::loadSpriteAnimation("turret_shoot.bmp", 1);

        Assets->storageSilo = asset::loadSprite("storage_silo.bmp");
        Assets->storageSiloFill = asset::loadSprite(
            "storage_silo_fill.bmp",
            "sprite.glsl",
            "progress_bar.glsl"
        );

        Assets->workerButton = asset::loadSprite("worker_button.bmp");
        Assets->workerButtonPressed = asset::loadSpriteAnimation("worker_button_pressed.bmp", 1);

        Assets->mineSiteButton = asset::loadSprite("mine_site_button.bmp");
        Assets->mineSiteButtonPressed = asset::loadSpriteAnimation("mine_site_button_pressed.bmp", 1);

        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");

        // NOTE: https://opengameart.org/content/since-2-am
        Assets->backgroundMusic = asset::loadSound("since_2_am.wav");
        Assets->resourceCollected = asset::loadSound("resource_collected.wav");
        Assets->resourceCollectedLow = asset::loadSound("resource_collected_low.wav");
        Assets->errorCollect = asset::loadSound("error.wav");
        Assets->resourceDropOff = asset::loadSound("resource_drop_off.wav");
        Assets->spawnMineSite = asset::loadSound("spawn_mine_site.wav");
        Assets->turretShoot = asset::loadSound("turret_shoot.wav");
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
            State->storageSilo,
            entity::Entity(
                "Storage Silo",
                render::RenderInfo(slurp::Globals->GameAssets->storageSilo, true, 0),
                physics::PhysicsInfo({275, 150}),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->storageSiloFill,
            entity::Entity(
                "Storage Silo Fill",
                render::RenderInfo(slurp::Globals->GameAssets->storageSiloFill, true, 0),
                physics::PhysicsInfo(State->storageSilo.physicsInfo.position),
                collision::CollisionInfo()
            )
        );

        registerEntity(
            State->mineSiteSpawner,
            mine_site::MineSiteSpawner()
        );

        new(&State->mineSites) entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES>(mine_site::MineSite());
        new(&State->mineSpots) types::deque_arena<slurp::Vec2<float> >();

        new(&State->workers) entity::EntityPool<worker::Worker, MAX_NUM_WORKERS>(worker::Worker());
        new(&State->targetableCorruptedWorkers) types::deque_arena<worker::Worker*>();

        new(&State->turrets) entity::EntityPool<turret::Turret, MAX_NUM_TURRETS>(turret::Turret());
        new(&State->turretsRangeIndicators) entity::EntityPool<entity::Entity, MAX_NUM_TURRETS>(
            entity::Entity(
                "Turret Range Indicator",
                render::RenderInfo(
                    Assets->turretRangeIndicatorSprite,
                    true,
                    0,
                    {0, 0}
                ),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        geometry::Shape buttonShape = {geometry::Rect, {25, 16}};
        slurp::Vec2<float> buttonRenderOffset = {0, -0.75f};
        registerEntity(
            State->workerButton,
            entity::Entity(
                "Worker Button",
                render::RenderInfo(
                    Assets->workerButton,
                    true,
                    UI,
                    buttonRenderOffset
                ),
                physics::PhysicsInfo({-50, 165}),
                collision::CollisionInfo(
                    true,
                    true,
                    buttonShape,
                    true
                )
            )
        );
        State->workerButton.renderInfo.animation = *Assets->workerButtonPressed;

        registerEntity(
            State->mineSiteButton,
            entity::Entity(
                "Mine Site Button",
                render::RenderInfo(
                    Assets->mineSiteButton,
                    true,
                    UI,
                    buttonRenderOffset
                ),
                physics::PhysicsInfo({0, 165}),
                collision::CollisionInfo(
                    true,
                    true,
                    buttonShape,
                    true
                )
            )
        );
        State->mineSiteButton.renderInfo.animation = *Assets->mineSiteButtonPressed;

        registerEntity(
            State->mouseCursor,
            mouse_cursor::MouseCursor()
        );
    }

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target) {
        return entity->physicsInfo.position.distanceSquaredTo(target) < entity->physicsInfo.speed * 0.01f;
    }
}
