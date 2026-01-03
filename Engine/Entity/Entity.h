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
        bool initialized;
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

        Entity(
            uint32_t id,
            std::string&& name,
            bool enabled,
            bool initialized,
            const render::RenderInfo& renderInfo,
            const physics::PhysicsInfo& physicsInfo,
            const collision::CollisionInfo& collisionInfo,
            bool shouldDestroy
        );

        void enable();

        // TODO: make these pure virtual or signal that they aren't implemented
        // TODO: any way to get around the overhead of virtual functions?
        virtual void initialize();

        virtual void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        );

        virtual void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState);

        void updatePhysics(float dt);

        virtual void update(float dt);

        virtual void onCollisionEnter(const collision::CollisionDetails& collisionDetails);

        virtual void onCollisionExit(const collision::CollisionDetails& collisionDetails);

        bool mouseHitTest(const slurp::Vec2<float>& location) const;

        void setTexture(const asset::Sprite* sprite);

        void setTexture(uint8_t spriteIndex, const asset::Sprite* sprite);

        void setAlpha(float alpha);

        void setAlpha(uint8_t spriteIndex, float alpha);

        void applyAlpha(float alpha);

        void applyAlpha(uint8_t spriteIndex, float alpha);

        void playAnimation(
            const asset::SpriteAnimation* animation,
            float totalDuration,
            bool shouldLoop = false,
            bool playReversed = false
        );

        void playAnimation(
            uint8_t spriteIndex,
            const asset::SpriteAnimation* animation,
            float totalDuration,
            bool shouldLoop = false,
            bool playReversed = false
        );

        void stopAnimation();

        void stopAnimation(uint8_t spriteIndex);

        Entity& operator=(const Entity& other);

        Entity& operator=(const Entity&& other);

        bool operator==(const Entity& other) const { return id == other.id; }

        virtual ~Entity() = default;
    };
}
