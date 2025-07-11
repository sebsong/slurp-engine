#include "Entity.h"

namespace slurp {
    Entity::Entity(Entity&& other) noexcept
        : id(std::move(other.id)),
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
        collisionInfo,
        nullptr
    ) {}

    Entity::Entity(
        std::string&& name,
        const geometry::Shape& renderShape,
        bool isCentered,
        render::Pixel color,
        const Vector2<int>& position,
        float speed,
        const collision::CollisionInfo& collisionInfo,
        const std::function<
            void(
                const MouseState& mouseState,
                const KeyboardState& keyboardState,
                const GamepadState (&controllerStates)[4]
            )
        >&& handleInput
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
        collisionInfo,
        std::move(handleInput)
    ) {}

    Entity::Entity(
        std::string&& name,
        bool enabled,
        const render::RenderShape& renderShape,
        const Vector2<int>& position,
        float speed,
        const Vector2<float>& direction,
        const collision::CollisionInfo& collisionInfo,
        const std::function<
            void(
                const MouseState& mouseState,
                const KeyboardState& keyboardState,
                const GamepadState (&controllerStates)[4]
            )
        >&& handleInput
    ): id(-1),
       name(std::move(name)),
       enabled(enabled),
       renderShape(renderShape),
       position(position),
       speed(speed),
       direction(direction),
       collisionInfo(collisionInfo),
       handleInput(std::move(handleInput)),
       shouldDestroy(false) {}
}
