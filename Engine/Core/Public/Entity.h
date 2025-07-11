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
        bool shouldDestroy;

        Entity(Entity&& other) noexcept;

        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position
        );

        Entity(
            std::string&& name,
            const geometry::Shape& renderShape,
            bool isCentered,
            render::Pixel color,
            const Vector2<int>& position,
            float speed,
            const collision::CollisionInfo& collisionInfo
        );

        Entity(
            std::string&& name,
            bool enabled,
            const render::RenderShape& renderShape,
            const Vector2<int>& position,
            float speed,
            const Vector2<float>& direction,
            const collision::CollisionInfo& collisionInfo
        );


        // TODO: make these pure virtual or signal that they aren't implemented
        // TODO: any way to get around the overhead of virtual functions?
        virtual void handleInput(
            const MouseState& mouseState,
            const KeyboardState& keyboardState,
            const GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
        ){};

        virtual void onCollisionEnter(const Entity* otherEntity){};

        virtual void onCollisionExit(const Entity* otherEntity){};

        virtual ~Entity() = default;

        bool operator==(const Entity& other) const {
            return id == other.id;
        }
    };
}
