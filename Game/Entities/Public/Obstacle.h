#pragma once
#include "Entity.h"

namespace game {
    class Obstacle: public slurp::Entity {
    public:
        Obstacle(std::string&& name, const geometry::Shape& shape, const slurp::Vector2<int>& position);
    };
}
