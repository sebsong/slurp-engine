#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6

namespace slurp {
    struct Entity;
    struct Tilemap;
}

namespace update {
    struct CollisionSquare {
        int radius;
    };

    void updatePosition(slurp::Entity& entity, const slurp::Tilemap& tilemap, float dt);
    void updatePosition(slurp::Entity& entity, const std::iterator_traits<slurp::Entity&>& allEntities, float dt);
}
