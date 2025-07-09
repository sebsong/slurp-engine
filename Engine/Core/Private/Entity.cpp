#include "Entity.h"

namespace slurp {
    Entity::Entity(
        const std::string&& name,
        const Vector2<int>& position,
        bool centerPosition,
        const geometry::Shape& renderShape,
        render::Pixel color
    ): Entity(
        std::move(name),
        position,
        centerPosition,
        renderShape,
        color,
        collision::CollisionInfo()
    ) {}

    Entity::Entity(
        const std::string&& name,
        const Vector2<int>& position,
        bool centerPosition,
        const geometry::Shape& renderShape,
        render::Pixel color,
        const collision::CollisionInfo& collisionInfo
    ): id(-1),
       name(std::move(name)),
       enabled(true),
       renderShape(render::RenderShape(
           renderShape,
           centerPosition ? -renderShape.dimensions / 2 : Vector2<int>::Zero,
           color
       )),
       position(position),
       speed({}),
       direction({}),
       collisionInfo(collisionInfo),
       shouldDestroy(false) {}
}
