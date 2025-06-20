#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6

namespace slurp {
    struct Entity;
}

namespace update {
    void updatePosition(slurp::Entity& entity, const std::iterator_traits<slurp::Entity&>& allEntities, float dt);
}
