#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6

namespace update
{
    struct Entity;
    struct Tilemap;
    void updatePosition(Entity& entity, const Tilemap& tilemap, const float& dt);
}
