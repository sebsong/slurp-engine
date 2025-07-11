#pragma once
#include "Vector.h"
#include "Input.h"
#include "Render.h"
#include "Collision.h"


#include <functional>

namespace slurp {
    struct Entity {
        uint32_t id;
        std::string name;
        bool enabled;
        render::RenderShape renderShape;
        Vector2<int> position;
        float speed;
        Vector2<float> direction;
        collision::CollisionInfo collisionInfo;
        const std::function<
            void(const MouseState& mouseState,
                 const KeyboardState& keyboardState,
                 const GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS])
        > handleInput;
        bool shouldDestroy;

        Entity(Entity&& other) noexcept;

        /* No collision, cosmetic */
        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position
        );

        /* Collision, no input, non-player */
        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position,
            float speed,
            const collision::CollisionInfo& collisionInfo
        );

        /* Player controlled */
        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position,
            float speed,
            const collision::CollisionInfo& collisionInfo,
            const std::function<
                void(const MouseState& mouseState,
                     const KeyboardState& keyboardState,
                     const GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS])
            >&& handleInput
        );

        Entity(
            std::string&& name,
            bool enabled,
            const render::RenderShape& renderShape,
            const Vector2<int>& position,
            float speed,
            const Vector2<float>& direction,
            const collision::CollisionInfo& collisionInfo,
            const std::function<
                void(const MouseState& mouseState,
                     const KeyboardState& keyboardState,
                     const GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS])
            >&& handleInput
        );

        virtual ~Entity() = default;

        bool operator==(const Entity& other) const {
            return id == other.id;
        }
    };
}
