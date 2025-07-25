#include "Global.h"
#include "Game.h"

namespace global {
    Global::Global(): Entity(
        "Global",
        render::RenderInfo(),
        physics::PhysicsInfo(),
        collision::CollisionInfo()
    ) {}

    void Global::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (
            (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) ||
            keyboardState.isDown(slurp::KeyboardCode::ESC)
        ) { game::GlobalPlatformDll->shutdown(); }
    }

    void Global::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            game::GlobalPlatformDll->shutdown();
        }
    }
}
