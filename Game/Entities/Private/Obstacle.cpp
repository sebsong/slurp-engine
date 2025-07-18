#include "Obstacle.h"

#include "Game.h"

namespace game {
    Obstacle::Obstacle(
        std::string&& name,
        const geometry::Shape& shape,
        const slurp::Vector2<int>& position
    ): Entity(
        std::move(name),
        shape,
        false,
        getColor(5),
        position,
        0,
        collision::CollisionInfo(
            true,
            false,
            shape,
            false
        )
    ) {}
}
