#include "Game.h"

#include "EntityManager.h"

#include "Obstacle.cpp"
#include "Player.cpp"
#include "MouseCursor.cpp"

namespace game {
    // TODO: maybe move color palette handling to its own module
    static slurp::GameState* GlobalGameState;
    static render::ColorPalette GlobalColorPalette;

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static const slurp::Vector2 EnemyStartPos = {400, 200};
    static const slurp::Vector2 EnemyPosOffset = {100, 0};
    static constexpr int BaseEnemySizePixels = 20;
    static constexpr render::ColorPaletteIdx EnemyColorPalletIdx = 4;
    static constexpr int BaseEnemySpeed = 200;
    static constexpr float BaseEnemyDirectionChangeDelay = 2;
    static constexpr float EnemyDirectionChangeDelayDelta = 1.5;

    static constexpr int ProjectileSizePixels = 15;
    static constexpr render::ColorPaletteIdx ProjectileColorPalletIdx = 1;
    static constexpr int BaseProjectileSpeed = 500;

    static constexpr int ColorPaletteSwatchSize = 15;
    static const slurp::Vector2 ColorPalettePosition = {1155, 702};

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

    static void setRandomDirection(slurp::Entity* entity) {
        float randX = random::randomFloat(-1, 1);
        float randY = random::randomFloat(-1, 1);
        entity->direction = slurp::Vector2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay() {
        float minDelay = BaseEnemyDirectionChangeDelay - EnemyDirectionChangeDelayDelta;
        float maxDelay = BaseEnemyDirectionChangeDelay + EnemyDirectionChangeDelayDelta;
        return random::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(slurp::Entity* enemy) {
        setRandomDirection(enemy);
        timer::delay(
            getRandomDirectionChangeDelay(),
            [&] {
                startUpdateEnemyDirection(enemy);
            }
        );
    }

    void initGame(slurp::GameState& gameState, slurp::EntityManager& entityManager) {
        GlobalGameState = &gameState;
        GlobalColorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(GlobalGameState->randomSeed);

        registerEntity(
            entityManager,
            GlobalGameState->background,
            slurp::Entity(
                "Background",
                {geometry::Rect, {1280, 720}},
                false,
                getColor(7),
                {0, 0}
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

        registerEntity(
            entityManager,
            GlobalGameState->player,
            Player()
        );

        registerEntity(
            entityManager,
            GlobalGameState->mouseCursor,
            MouseCursor()
        );

        // TODO: move some of these to separate classes/files
        geometry::Shape enemyShape = {geometry::Rect, {BaseEnemySizePixels, BaseEnemySizePixels}};
        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->enemies[i],
                slurp::Entity(
                    "Enemy" + std::to_string(i),
                    enemyShape,
                    true,
                    getColor(EnemyColorPalletIdx),
                    EnemyStartPos + (EnemyPosOffset * i),
                    BaseEnemySpeed,
                    collision::CollisionInfo(
                        false,
                        enemyShape,
                        true
                    )
                )
            );
            // startUpdateEnemyDirection(enemy); // TODO: re-enable this
        }

        geometry::Shape projectileShape = {geometry::Rect, {ProjectileSizePixels, ProjectileSizePixels}};
        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->projectiles[i],
                slurp::Entity(
                    "Projectile" + std::to_string(i),
                    projectileShape,
                    true,
                    getColor(ProjectileColorPalletIdx),
                    slurp::Vector2<int>::Zero,
                    BaseProjectileSpeed,
                    collision::CollisionInfo(
                        false,
                        projectileShape,
                        true
                    )
                )
            );
        }

        for (uint8_t i = 0; i < COLOR_PALETTE_SIZE; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->colorPaletteSwatch[i],
                slurp::Entity(
                    "ColorPaletteSwatch" + std::to_string(i),
                    {geometry::Rect, {ColorPaletteSwatchSize, ColorPaletteSwatchSize}},
                    false,
                    getColor(i),
                    ColorPalettePosition + slurp::Vector2{i * ColorPaletteSwatchSize, 0}
                )
            );
        }
    }
}
