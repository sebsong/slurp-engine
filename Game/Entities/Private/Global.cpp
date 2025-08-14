#include "Global.h"
#include "Game.h"

namespace global {
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static const audio::Sound bgm = audio::loadSound("bgm.wav");

    Global::Global(): Entity(
        "Global",
        render::RenderInfo(),
        physics::PhysicsInfo(),
        collision::CollisionInfo()
    ) {}

    void Global::initialize() {
        Entity::initialize();

        // game::GlobalSoundManager->playSound(bgm, 1, true);
    }

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
