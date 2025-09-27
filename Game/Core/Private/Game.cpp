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
        GlobalGameAssets->colorPalette = asset::loadColorPalette(ColorPaletteHexFileName);

        GlobalGameAssets->enemySprite = render::loadSprite(enemy::SpriteFileName);
        GlobalGameAssets->mouseCursorSprite = render::loadSprite(mouse_cursor::SpriteFileName);
        GlobalGameAssets->playerSprite = render::loadSprite(player::SpriteFileName);
        GlobalGameAssets->playerParrySprite = render::loadSprite(player::ParrySpriteFileName);
        GlobalGameAssets->projectileSprite = render::loadSprite(projectile::SpriteFileName);
        GlobalGameAssets->projectileParriedSprite = render::loadSprite(projectile::ParriedSpriteFileName);

        GlobalGameAssets->backgroundMusic = audio::loadSound(projectile::SoundFileName);
        GlobalGameAssets->projectileHitSound = audio::loadSound(projectile::SoundFileName);
    }

    void initGame(GameAssets& gameAssets, GameState& gameState) {
        GlobalGameAssets = &gameAssets;
        GlobalGameState = &gameState;

        loadAssets();

        slurp::GlobalRenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

#if 1
        const render::Vertex triangleVertices[] = {
            render::Vertex{{0.5f, 0.5f, 0.f}, {1, 1}},
            render::Vertex{{-0.5f, 0.5f, 0.f}, {0, 1}},
            render::Vertex{{-0.5f, -0.5f, 0.f}, {0, 0}},
            render::Vertex{{0.5f, -0.5f, 0.f}, {1, 0}},
        };
        const uint32_t triangleElements[] = {0, 1, 2, 2, 3, 0};
        render::object_id vertexArrayId = slurp::GlobalRenderApi->genElementArrayBuffer(
            triangleVertices,
            4,
            triangleElements,
            6
        );
        asset::Bitmap bitmap = asset::loadBitmapFile("player.bmp");
        render::object_id textureId = slurp::GlobalRenderApi->createTexture(bitmap);
        render::object_id shaderProgramId = slurp::GlobalRenderApi->loadShaderProgram(
            "tutorial.glsl",
            "tutorial.glsl"
        );
        registerEntity(
            GlobalGameState->triangle,
            slurp::Entity(
                "Triangle",
                render::RenderInfo(
                    GlobalGameAssets->playerSprite,
                    true
                ),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );
#endif

        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(GlobalGameState->randomSeed);

        registerEntity(
            GlobalGameState->global,
            global::Global()
        );

        registerEntity(
            GlobalGameState->background,
            slurp::Entity(
                "Background",
                render::RenderInfo(
                    render::RenderShape{
                        {geometry::Rect, {1280, 720}},
                        getColor(2)
                    },
                    false
                ),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );
        geometry::Shape wallUpShape = {geometry::Rect, {1500, 20}};
        registerEntity(
            GlobalGameState->wallUp,
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, 0}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        registerEntity(
            GlobalGameState->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, 700}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            GlobalGameState->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {0, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            GlobalGameState->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {1260, 0}
            )
        );
        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        registerEntity(
            GlobalGameState->obstacle1,
            obstacle::Obstacle(
                "Obstacle1",
                obstacle1Shape,
                {200, 500}
            )
        );
        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        registerEntity(
            GlobalGameState->obstacle2,
            obstacle::Obstacle(
                "Obstacle2",
                obstacle2Shape,
                {500, 400}
            )
        );

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

        // TODO: move some of these to separate classes/files
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

        registerEntity(
            GlobalGameState->testAlpha,
            slurp::Entity(
                "testAlpha",
                render::RenderInfo(
                    render::RenderShape{
                        {geometry::Rect, {300, 200}},
                        render::withAlpha(getColor(6), .7)
                    },
                    true
                ),
                physics::PhysicsInfo(
                    {400, 525}
                ),
                collision::CollisionInfo()
            )
        );

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
