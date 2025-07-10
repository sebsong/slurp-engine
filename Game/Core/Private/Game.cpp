#include "Game.h"

#include "UpdateRenderPipeline.h"

#include "Player.cpp"

namespace game {
    // TODO: maybe move color palette handling to its own module
    static slurp::GameState* GlobalGameState;
    static render::ColorPalette GlobalColorPalette;

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static constexpr int MouseCursorSizePixels = 10;
    static constexpr render::ColorPaletteIdx MouseCursorColorPalletIdx = 1;

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        return GlobalColorPalette.colors[colorPaletteIdx];
    }

    static void registerEntity(
        slurp::UpdateRenderPipeline& pipeline,
        slurp::Entity& entityLocation,
        slurp::Entity&& entity
    ) {
        // TODO: this pattern is a little weird
        new(&entityLocation) slurp::Entity(std::move(entity));
        pipeline.registerEntity(entityLocation);
    }

    void initGame(slurp::GameState& gameState, slurp::UpdateRenderPipeline& updateRenderPipeline) {
        GlobalGameState = &gameState;
        GlobalColorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        registerEntity(
            updateRenderPipeline,
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
            updateRenderPipeline,
            GlobalGameState->wallUp,
            slurp::Entity(
                "WallUp",
                wallUpShape,
                false,
                getColor(5),
                {0, 0},
                0,
                collision::CollisionInfo(
                    true,
                    wallUpShape,
                    false
                )
            )
        );

        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        registerEntity(
            updateRenderPipeline,
            GlobalGameState->wallDown,
            slurp::Entity(
                "WallDown",
                wallDownShape,
                false,
                getColor(5),
                {0, 700},
                0,
                collision::CollisionInfo(
                    true,
                    wallDownShape,
                    false
                )
            )
        );

        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            updateRenderPipeline,
            GlobalGameState->wallLeft,
            slurp::Entity(
                "WallLeft",
                wallLeftShape,
                false,
                getColor(5),
                {0, 0},
                0,
                collision::CollisionInfo(
                    true,
                    wallLeftShape,
                    false
                )
            )
        );

        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        registerEntity(
            updateRenderPipeline,
            GlobalGameState->wallRight,
            slurp::Entity(
                "WallRight",
                wallRightShape,
                false,
                getColor(5),
                {1260, 0},
                0,
                collision::CollisionInfo(
                    true,
                    wallRightShape,
                    false
                )
            )
        );

        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        registerEntity(
            updateRenderPipeline,
            GlobalGameState->obstacle1, slurp::Entity(
                "Obstacle1",
                obstacle1Shape,
                true,
                getColor(5),
                {200, 500},
                0,
                collision::CollisionInfo(
                    true,
                    obstacle1Shape,
                    true
                )
            )
        );

        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        registerEntity(
            updateRenderPipeline,
            GlobalGameState->obstacle2,
            slurp::Entity(
                "Obstacle2",
                obstacle2Shape,
                true,
                getColor(5),
                {500, 500},
                0,
                collision::CollisionInfo(
                    true,
                    obstacle2Shape,
                    true
                )
            )
        );

        registerEntity(
            updateRenderPipeline,
            GlobalGameState->player,
            Player()
        );

        registerEntity(
            updateRenderPipeline,
            GlobalGameState->mouseCursor,
            slurp::Entity(
                "MouseCursor",
                {geometry::Rect, {MouseCursorSizePixels, MouseCursorSizePixels}},
                true,
                getColor(MouseCursorColorPalletIdx),
                {}
            )
        );
    }
}
