#include "Global.h"
#include "Game.h"

namespace game {
    Global::Global(): Entity(
        "Global",
        {},
        {},
        {},
        {}
    ) {}

    void Global::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (
            (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) ||
            keyboardState.isDown(slurp::KeyboardCode::ESC)
        ) { GlobalPlatformDll->shutdown(); }
    }

    void Global::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            GlobalPlatformDll->shutdown();
        }
    }
}
