#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityManager.h"

#include "Global.cpp"
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
        return GlobalGameAssets->colorPalette.colors[colorPaletteIdx];
    }

    template<typename T>
    static void registerEntity(
        T& entityLocation,
        T&& entity
    ) {
        // TODO: this pattern is a little weird, also need to know to include new properties in the move constructor
        new(&entityLocation) T(std::forward<T>(entity));
        slurp::GlobalEntityManager->registerEntity(entityLocation);
    }

    static void loadAssets() {
        GlobalGameAssets->colorPalette = slurp::GlobalAssetLoader->loadColorPalette(ColorPaletteHexFileName);

        GlobalGameAssets->borderSprite = slurp::GlobalAssetLoader->loadSprite("border.bmp");
        GlobalGameAssets->enemySprite = slurp::GlobalAssetLoader->loadSprite("enemy.bmp");
        GlobalGameAssets->mouseCursorSprite = slurp::GlobalAssetLoader->loadSprite("mouse_cursor.bmp");
        GlobalGameAssets->playerSprite = slurp::GlobalAssetLoader->loadSprite("player.bmp");
        GlobalGameAssets->playerParrySprite = slurp::GlobalAssetLoader->loadSprite("player_parry.bmp");
        GlobalGameAssets->projectileSprite = slurp::GlobalAssetLoader->loadSprite("projectile.bmp");
        GlobalGameAssets->projectileParriedSprite = slurp::GlobalAssetLoader->loadSprite("projectile_parried.bmp");

        GlobalGameAssets->backgroundMusic = slurp::GlobalAssetLoader->loadSound(global::BackgroundMusicSoundFileName);
        GlobalGameAssets->projectileHitSound = slurp::GlobalAssetLoader->loadSound(projectile::SoundFileName);
    }

    void initGame(GameAssets* gameAssets, GameState* gameState) {
        GlobalGameAssets = gameAssets;
        GlobalGameState = gameState;

        loadAssets();

        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));
        slurp_random::setRandomSeed(GlobalGameState->randomSeed);

        slurp::GlobalRenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

        registerEntity(
            GlobalGameState->global,
            global::Global()
        );

        registerEntity(
            GlobalGameState->border,
            slurp::Entity(
                "Border",
                render::RenderInfo(GlobalGameAssets->borderSprite, true),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

        // registerEntity(
        //     GlobalGameState->background,
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
            GlobalGameState->wallUp,
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {CAMERA_WORLD_WIDTH, 20}};
        registerEntity(
            GlobalGameState->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, -CAMERA_WORLD_HEIGHT_MAX}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            GlobalGameState->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {-CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, CAMERA_WORLD_HEIGHT}};
        registerEntity(
            GlobalGameState->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {CAMERA_WORLD_WIDTH_MAX, 0}
            )
        );
        // geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        // registerEntity(
        //     GlobalGameState->obstacle1,
        //     obstacle::Obstacle(
        //         "Obstacle1",
        //         obstacle1Shape,
        //         {200, 500}
        //     )
        // );
        // geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        // registerEntity(
        //     GlobalGameState->obstacle2,
        //     obstacle::Obstacle(
        //         "Obstacle2",
        //         obstacle2Shape,
        //         {500, 400}
        //     )
        // );

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                GlobalGameState->projectiles[i],
                projectile::Projectile(i)
            );
        }

        registerEntity(
            GlobalGameState->player,
            player::Player()
        );

        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                GlobalGameState->enemies[i],
                enemy::Enemy(i)
            );
        }

        registerEntity(
            GlobalGameState->mouseCursor,
            mouse_cursor::MouseCursor()
        );

        // registerEntity(
        //     GlobalGameState->testAlpha,
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
                GlobalGameState->colorPaletteSwatch[i],
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
