#include "Game.h"

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
    static const slurp::Vector2 ColorPalettePosition = {1140, 702};

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        return GlobalColorPalette.colors[colorPaletteIdx];
    }

    template<typename T>
    static void registerEntity(
        slurp::EntityManager& entityManager,
        T& entityLocation,
        T&& entity
    ) {
        // TODO: this pattern is a little weird, also need to know to include new properties in the move constructor
        new(&entityLocation) T(std::move(entity));
        entityManager.registerEntity(entityLocation);
    }

    void initGame(
        const platform::PlatformDll& platformDll,
        GameState& gameState,
        slurp::EntityManager& entityManager
    ) {
        GlobalPlatformDll = &platformDll;
        GlobalGameState = &gameState;
        GlobalColorPalette = render::loadColorPalette(ColorPaletteHexFileName);

        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(GlobalGameState->randomSeed);

        registerEntity(
            entityManager,
            GlobalGameState->global,
            Global()
        );

        registerEntity(
            entityManager,
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
            entityManager,
            GlobalGameState->wallUp,
            Obstacle(
                "WallUp",
                wallUpShape,
                {0, 0}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        registerEntity(
            entityManager,
            GlobalGameState->wallDown,
            Obstacle(
                "WallDown",
                wallDownShape,
                {0, 700}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            entityManager,
            GlobalGameState->wallLeft,
            Obstacle(
                "WallLeft",
                wallLeftShape,
                {0, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            entityManager,
            GlobalGameState->wallRight,
            Obstacle(
                "WallRight",
                wallRightShape,
                {1260, 0}
            )
        );
        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        registerEntity(
            entityManager,
            GlobalGameState->obstacle1,
            Obstacle(
                "Obstacle1",
                obstacle1Shape,
                {200, 500}
            )
        );
        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        registerEntity(
            entityManager,
            GlobalGameState->obstacle2,
            Obstacle(
                "Obstacle2",
                obstacle2Shape,
                {500, 400}
            )
        );

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->projectiles[i],
                Projectile(i)
            );
        }

        registerEntity(
            entityManager,
            GlobalGameState->player,
            Player()
        );

        // TODO: move some of these to separate classes/files
        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->enemies[i],
                Enemy(i)
            );
        }

        registerEntity(
            entityManager,
            GlobalGameState->mouseCursor,
            MouseCursor()
        );

        registerEntity(
            entityManager,
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
                entityManager,
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
                        ColorPalettePosition + slurp::Vector2{i * ColorPaletteSwatchSize, 0}
                    ),
                    collision::CollisionInfo()
                )
            );
        }
    }
}
