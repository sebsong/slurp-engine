#pragma once

namespace game {
    static const platform::PlatformDll* GlobalPlatformDll;
    static slurp::GameState* GlobalGameState;
    static render::ColorPalette GlobalColorPalette;

    void init(slurp::GameState& gameState, slurp::EntityManager& entityManager);

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
