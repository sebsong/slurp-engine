#pragma once
#include "Entity.h"

namespace global {
    class GameGlobal final : public entity::Entity {
    public:
        GameGlobal();

    private:
        void initialize() override;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;
    };
}
