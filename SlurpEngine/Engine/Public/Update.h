#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6

namespace slurp {
    struct Entity;
    struct Tilemap;
}

namespace update {
    void updatePosition(slurp::Entity& entity, const slurp::Tilemap& tilemap, float dt);
}
