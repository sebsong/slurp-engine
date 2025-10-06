#include "Obstacle.h"

#include "Game.h"

namespace obstacle {
    Obstacle::Obstacle(
        std::string&& name,
        const geometry::Shape& shape,
        const slurp::Vec2<float>& position
    ): Entity(
        std::move(name),
        render::RenderInfo(
            render::RenderShape{shape, game::getColor(7)},
            true
        ),
        physics::PhysicsInfo(position),
        collision::CollisionInfo(
            true,
            false,
            shape,
            true
        )
    ) {}
}
