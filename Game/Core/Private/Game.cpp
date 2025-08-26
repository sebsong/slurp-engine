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
    static const slurp::Vec2 ColorPalettePosition = {1140, 702};

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx) {
        ASSERT(colorPaletteIdx < COLOR_PALETTE_SIZE);
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
        GameState& gameState,
        slurp::EntityManager& entityManager,
        audio::SoundManager& soundManager,
        const platform::PlatformDll& platformDll,
        const render::RenderApi& renderApi
    ) {
        GlobalPlatformDll = &platformDll;
        GlobalRenderApi = &renderApi;
        GlobalGameState = &gameState;
        GlobalSoundManager = &soundManager;
        GlobalColorPalette = asset::loadColorPalette(ColorPaletteHexFileName);

#if 1
        const slurp::Vec3<float> triangleVertices[] = {
            // Vertex 0
            slurp::Vec3(-0.5f, 0.75f, 0.f),
            // slurp::Vec3(1.f, 0.f, 0.f),

            // Vertex 1
            slurp::Vec3(-1.f, -0.75f, 0.f),
            // slurp::Vec3(0.f, 1.f, 0.f),

            // Vertex 2
            slurp::Vec3(0.f, -0.75f, 0.f),
            // slurp::Vec3(0.f, 0.f, 1.f),

            // slurp::Vec3(0.f, 0.75f, 0.f),
            // slurp::Vec3(-0.75f, -0.75f, 0.f),
            // slurp::Vec3(0.75f, -0.75f, 0.f),
        };
        render::vertexArrayId vertexArrayId = GlobalRenderApi->genArrayBuffer(triangleVertices, 3);
        render::shader_program_id shaderProgramId = GlobalRenderApi->loadShaderProgram(
            "tutorial.glsl",
            "tutorial.glsl"
        );
        registerEntity(
            entityManager,
            GlobalGameState->triangle,
            slurp::Entity(
                "Triangle",
                render::RenderInfo(
                    open_gl::OpenGLRenderInfo(
                        vertexArrayId,
                        3,
                        shaderProgramId
                    ),
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
            entityManager,
            GlobalGameState->global,
            global::Global()
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
            obstacle::Obstacle(
                "WallUp",
                wallUpShape,
                {0, 0}
            )
        );
        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        registerEntity(
            entityManager,
            GlobalGameState->wallDown,
            obstacle::Obstacle(
                "WallDown",
                wallDownShape,
                {0, 700}
            )
        );
        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            entityManager,
            GlobalGameState->wallLeft,
            obstacle::Obstacle(
                "WallLeft",
                wallLeftShape,
                {0, 0}
            )
        );
        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            entityManager,
            GlobalGameState->wallRight,
            obstacle::Obstacle(
                "WallRight",
                wallRightShape,
                {1260, 0}
            )
        );
        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        registerEntity(
            entityManager,
            GlobalGameState->obstacle1,
            obstacle::Obstacle(
                "Obstacle1",
                obstacle1Shape,
                {200, 500}
            )
        );
        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        registerEntity(
            entityManager,
            GlobalGameState->obstacle2,
            obstacle::Obstacle(
                "Obstacle2",
                obstacle2Shape,
                {500, 400}
            )
        );

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->projectiles[i],
                projectile::Projectile(i)
            );
        }

        registerEntity(
            entityManager,
            GlobalGameState->player,
            player::Player()
        );

        // TODO: move some of these to separate classes/files
        for (int i = 0; i < NUM_ENEMIES; i++) {
            registerEntity(
                entityManager,
                GlobalGameState->enemies[i],
                enemy::Enemy(i)
            );
        }

        registerEntity(
            entityManager,
            GlobalGameState->mouseCursor,
            mouse_cursor::MouseCursor()
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
                        ColorPalettePosition + slurp::Vec2{i * ColorPaletteSwatchSize, 0}
                    ),
                    collision::CollisionInfo()
                )
            );
        }
    }
}
