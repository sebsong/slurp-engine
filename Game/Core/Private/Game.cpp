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

    void init(slurp::GameState& gameState, slurp::UpdateRenderPipeline& updateRenderPipeline) {
        GlobalGameState = &gameState;
        GlobalColorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        new(&GlobalGameState->background) slurp::Entity(
            "Background",
            {geometry::Rect, {1280, 720}},
            false,
            getColor(7),
            {0, 0}
        );
        updateRenderPipeline.registerEntity(GlobalGameState->background);

        geometry::Shape wallUpShape = {geometry::Rect, {1500, 20}};
        new(&GlobalGameState->wallUp) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->wallUp);

        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        new(&GlobalGameState->wallDown) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->wallDown);

        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        new(&GlobalGameState->wallLeft) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->wallLeft);

        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        new(&GlobalGameState->wallRight) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->wallRight);

        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        new(&GlobalGameState->obstacle1) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->obstacle1);

        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        new(&GlobalGameState->obstacle2) slurp::Entity(
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
        );
        updateRenderPipeline.registerEntity(GlobalGameState->obstacle2);

        new(&GlobalGameState->player) Player();
        updateRenderPipeline.registerEntity(GlobalGameState->player);

        new(&GlobalGameState->mouseCursor) slurp::Entity(
            "MouseCursor",
            {geometry::Rect, {MouseCursorSizePixels, MouseCursorSizePixels}},
            true,
            getColor(MouseCursorColorPalletIdx),
            {}
        );
        updateRenderPipeline.registerEntity(GlobalGameState->mouseCursor);
    }
}
