#pragma once
#include "Input.h"
#include "Collision.h"
#include "Physics.h"
#include "RenderInfo.h"

namespace entity {
    struct Entity {
        uint32_t id;
        std::string name;
        bool enabled;
        render::RenderInfo renderInfo;
        physics::PhysicsInfo physicsInfo;
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        Entity() noexcept;

        Entity(const Entity& other) noexcept;

        Entity(Entity&& other) noexcept;

        Entity(std::string&& name);

        Entity(
            std::string&& name,
            const render::RenderInfo& renderInfo,
            const physics::PhysicsInfo& physicsInfo,
            const collision::CollisionInfo& collisionInfo
        );

        // TODO: make these pure virtual or signal that they aren't implemented
        // TODO: any way to get around the overhead of virtual functions?
        virtual void initialize() {};

        void enable() {
            enabled = true;
            initialize();
        }

        virtual void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) {};

        virtual void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {};

        void updatePhysics(float dt);

        virtual void update(float dt) {};

        virtual void onCollisionEnter(const collision::CollisionDetails& collisionDetails) {};

        virtual void onCollisionExit(const collision::CollisionDetails& collisionDetails) {};

        virtual ~Entity() = default;

        bool operator==(const Entity& other) const { return id == other.id; }
    };
}
