#pragma once
#include "Entity.h"

namespace obstacle {
    class Obstacle: public slurp::Entity {
    public:
        Obstacle(std::string&& name, const geometry::Shape& shape, const slurp::Vector2<float>& position);
    };
}
