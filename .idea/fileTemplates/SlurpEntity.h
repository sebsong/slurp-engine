#pragma once
\#include "Entity.h"

// TODO: include ${CLASS_NAME}.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

#set ($CLASS_NAME_SNAKE_CASE = $CLASS_NAME.replaceAll("([a-z])([A-Z])", "$1_$2").toLowerCase())
namespace ${CLASS_NAME_SNAKE_CASE} {
    class ${CLASS_NAME} final : public entity::Entity {
    public:
        ${CLASS_NAME}();

    private:
        void initialize() override;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
        
        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;
        
        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override {};

        void onCollisionExit(const collision::CollisionDetails& collisionDetails) override {};
    };
}
