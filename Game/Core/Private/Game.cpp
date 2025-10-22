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
    // TODO: maybe move color palette handling to its own module
    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static constexpr int ColorPaletteSwatchSize = 15;
    static const slurp::Vec2 ColorPalettePosition = {1140, 702};

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx) {
        ASSERT(colorPaletteIdx < COLOR_PALETTE_SIZE);
        return slurp::Globals->GameAssets->colorPalette.colors[colorPaletteIdx];
    }

    template<typename T>
    static void registerEntity(
        T& entityLocation,
        T&& entity
    ) {
        // TODO: this pattern is a little weird, also need to know to include new properties in the move constructor
        new(&entityLocation) T(std::forward<T>(entity));
        slurp::Globals->EntityPipeline->registerEntity(entityLocation);
    }

    static void loadAssets() {
        slurp::Globals->GameAssets->colorPalette = slurp::Globals->AssetLoader->loadColorPalette(ColorPaletteHexFileName);

        slurp::Globals->GameAssets->borderSprite = slurp::Globals->AssetLoader->loadSprite("border.bmp");
        slurp::Globals->GameAssets->enemySprite = slurp::Globals->AssetLoader->loadSprite("enemy.bmp");
        slurp::Globals->GameAssets->mouseCursorSprite = slurp::Globals->AssetLoader->loadSprite("mouse_cursor.bmp");
        slurp::Globals->GameAssets->playerSprite = slurp::Globals->AssetLoader->loadSprite("player.bmp");
        slurp::Globals->GameAssets->playerParrySprite = slurp::Globals->AssetLoader->loadSprite("player_parry.bmp");
        slurp::Globals->GameAssets->projectileSprite = slurp::Globals->AssetLoader->loadSprite("projectile.bmp");
        slurp::Globals->GameAssets->projectileParriedSprite = slurp::Globals->AssetLoader->loadSprite("projectile_parried.bmp");

        slurp::Globals->GameAssets->backgroundMusic = slurp::Globals->AssetLoader->loadSound(global::BackgroundMusicSoundFileName);
        slurp::Globals->GameAssets->projectileHitSound = slurp::Globals->AssetLoader->loadSound(projectile::SoundFileName);
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

        // registerEntity(
        //     slurp::Globals->GameState->testAlpha,
        //     slurp::Entity(
        //         "testAlpha",
        //         render::RenderInfo(
        //             render::RenderShape{
        //                 {geometry::Rect, {300, 200}},
        //                 render::withAlpha(getColor(6), .7)
        //             },
        //             true
        //         ),
        //         physics::PhysicsInfo(
        //             {400, 525}
        //         ),
        //         collision::CollisionInfo()
        //     )
        // );

        for (uint8_t i = 0; i < COLOR_PALETTE_SIZE; i++) {
            registerEntity(
                slurp::Globals->GameState->colorPaletteSwatch[i],
                slurp::Entity(
                    "ColorPaletteSwatch" + std::to_string(i),
                    render::RenderInfo(
                        render::RenderShape{
                            {geometry::Rect, {ColorPaletteSwatchSize, ColorPaletteSwatchSize}},
                            getColor(i)
                        },
                        false
                    ),
                    physics::PhysicsInfo(
                        ColorPalettePosition + slurp::Vec2{i * ColorPaletteSwatchSize, 0}
                    ),
                    collision::CollisionInfo()
                )
            );
        }
    }
}
