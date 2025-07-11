#pragma once

namespace game {
    void init(slurp::GameState& gameState, slurp::EntityManager& entityManager);

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
