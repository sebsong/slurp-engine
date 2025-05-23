#pragma once
#include <Vector.hpp>

namespace slurp
{
    struct Entity
    {
        int size;
        float speed;
        Vector2<int> position;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
    };

    struct Tilemap;
    void updatePosition(Entity& entity, const Tilemap& tilemap, const float& dt);
}
