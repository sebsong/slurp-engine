#pragma once
#include "Vector.h"
#include "Input.h"
#include "Collision.h"
#include "Physics.h"
#include "RenderInfo.h"

namespace slurp {
    struct Entity {
        uint32_t id;
        std::string name;
        bool enabled;
        render::RenderInfo renderInfo;
        physics::PhysicsInfo physicsInfo;
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        Entity(Entity&& other) noexcept;

        Entity(
            std::string&& name,
            const render::RenderInfo& renderInfo,
            const physics::PhysicsInfo& physicsInfo,
            const collision::CollisionInfo& collisionInfo
        );

        // TODO: make these pure virtual or signal that they aren't implemented
        // TODO: any way to get around the overhead of virtual functions?
        virtual void initialize() {};

        virtual void handleMouseAndKeyboardInput(
            const MouseState& mouseState,
            const KeyboardState& keyboardState
        ) {};

        virtual void handleGamepadInput(uint8_t gamepadIndex, const GamepadState& gamepadState) {};

        void updatePhysics(float dt);

        virtual void update(float dt) {};

        virtual void onCollisionEnter(const collision::CollisionDetails& collisionDetails) {};

        virtual void onCollisionExit(const collision::CollisionDetails& collisionDetails) {};

        virtual ~Entity() = default;

        bool operator==(const Entity& other) const { return id == other.id; }
    };
}
