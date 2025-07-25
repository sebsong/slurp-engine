#include "Obstacle.h"

#include "Game.h"

namespace obstacle {
    Obstacle::Obstacle(
        std::string&& name,
        const geometry::Shape& shape,
        const slurp::Vector2<int>& position
    ): Entity(
        std::move(name),
        render::RenderInfo(
            render::RenderShape{shape, game::getColor(7)},
            false
        ),
        physics::PhysicsInfo(position),
        collision::CollisionInfo(
            true,
            false,
            shape,
            false
        )
    ) {}
}
