#pragma once

namespace game {
    void init(slurp::GameState& gameState, slurp::UpdateRenderPipeline& updateRenderPipeline);

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
