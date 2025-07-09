#include "Entity.h"

namespace slurp {
    Entity::Entity(
        const std::string&& name,
        const geometry::Shape& renderShape,
        bool isCentered,
        render::Pixel color,
        const Vector2<int>& position
    ): Entity(
        std::move(name),
        renderShape,
        isCentered,
        color,
        position,
        0,
        collision::CollisionInfo()
    ) {}

    Entity::Entity(
        const std::string&& name,
        const geometry::Shape& renderShape,
        bool isCentered,
        render::Pixel color,
        const Vector2<int>& position,
        float speed,
        const collision::CollisionInfo& collisionInfo
    ): Entity(
        std::move(name),
        true,
        render::RenderShape(
            renderShape,
            isCentered ? -renderShape.dimensions / 2 : Vector2<int>::Zero,
            color
        ),
        position,
        speed,
        Vector2<float>::Zero,
        collisionInfo
    ) {}

    Entity::Entity(
        const std::string&& name,
        bool enabled,
        const render::RenderShape& renderShape,
        const Vector2<int>& position,
        float speed,
        const Vector2<float>& direction,
        const collision::CollisionInfo& collisionInfo
    ): id(-1),
       name(std::move(name)),
       enabled(enabled),
       renderShape(renderShape),
       position(position),
       speed(speed),
       direction(direction),
       collisionInfo(collisionInfo),
       shouldDestroy(false) {}
}
