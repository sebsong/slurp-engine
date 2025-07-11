#pragma once

#include "Entity.h"
#include "Input.h"

namespace game {
    class Player final : public slurp::Entity {
    public:
        Player();

        bool isParryActive;

    private:
        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;
        void onCollisionEnter(const Entity* other) override;
        void onCollisionExit(const Entity* other) override;

        void activateParry();
        void deactivateParry();
    };
}
