#pragma once
\#include "Entity.h"

// TODO: include ${CLASS_NAME}.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace game {
    class ${CLASS_NAME} final : public slurp::Entity {
    public:
        ${CLASS_NAME}();

    private:
        void initialize() override;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
        
        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;
        
        void onCollisionEnter(const Entity* other) override;
        
        void onCollisionExit(const Entity* other) override;
    };
}
