#include "Entity.h"

namespace slurp {
    Entity::Entity(Entity&& other): id(std::move(other.id)),
                                    name(std::move(other.name)),
                                    enabled(std::move(other.enabled)),
                                    renderShape(std::move(other.renderShape)),
                                    position(std::move(other.position)),
                                    speed(std::move(other.speed)),
                                    direction(std::move(other.direction)),
                                    collisionInfo(std::move(other.collisionInfo)),
                                    shouldDestroy(std::move(other.shouldDestroy)) {}


    Entity::Entity(
        std::string&& name,
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
        std::string&& name,
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
        std::string&& name,
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

    Entity Entity::createWithoutCollision(
        std::string&& name,
        const geometry::Shape& renderShape,
        bool isCentered,
        render::Pixel color,
        const Vector2<int>& position
    ) {
        return Entity(
            std::move(name),
            renderShape,
            isCentered,
            color,
            position,
            0,
            collision::CollisionInfo()
        );
    }

    Entity Entity::createWithCollision(
        std::string&& name,
        const geometry::Shape& renderShape,
        bool isCentered,
        render::Pixel color,
        const Vector2<int>& position,
        float speed,
        const collision::CollisionInfo& collisionInfo
    ) {
        return Entity(
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
        );
    }
}
