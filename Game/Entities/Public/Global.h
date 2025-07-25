#pragma once
#include "Entity.h"

namespace global {
    class Global final : public slurp::Entity {
    public:
        Global();

    private:
        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;
    };
}
