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

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        return GlobalColorPalette.colors[colorPaletteIdx];
    }

    void init(slurp::GameState& gameState, slurp::UpdateRenderPipeline& updateRenderPipeline) {
        GlobalGameState = &gameState;
        GlobalColorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        new(&GlobalGameState->player) Player();
        updateRenderPipeline.registerEntity(GlobalGameState->player);
    }
}
