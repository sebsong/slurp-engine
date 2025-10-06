#include "Global.h"
#include "Game.h"

namespace global {
    static const float GlobalVolume = 0.5f;
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static constexpr const char* BackgroundMusicSoundFileName = "bgm.wav";

    // NOTE: https://opengameart.org/content/since-2-am
    static constexpr const char* BackgroundMusicSoundFileName = "since_2_am.wav";

    Global::Global(): Entity(
        "Global",
        render::RenderInfo(),
        physics::PhysicsInfo(),
        collision::CollisionInfo()
    ) {}

    void Global::initialize() {
        Entity::initialize();

        slurp::GlobalSoundManager->setGlobalVolume(GlobalVolume);
        // slurp::GlobalSoundManager->playSound(game::GlobalGameAssets->backgroundMusic, 0.5, true);
    }

    void Global::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (
            (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) ||
            keyboardState.isDown(slurp::KeyboardCode::ESC)
        ) { slurp::GlobalPlatformDll->shutdown(); }
    }

    void Global::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            slurp::GlobalPlatformDll->shutdown();
        }
    }
}
