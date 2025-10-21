#include "GameGlobal.h"
#include "Game.h"

namespace global {
    static const float GlobalVolume = 0.3f;
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static constexpr const char* BackgroundMusicSoundFileName = "bgm.wav";

    // NOTE: https://opengameart.org/content/since-2-am
    static constexpr const char* BackgroundMusicSoundFileName = "since_2_am.wav";

    GameGlobal::GameGlobal(): Entity(
        "Global",
        render::RenderInfo(),
        physics::PhysicsInfo(),
        collision::CollisionInfo()
    ) {}

    void GameGlobal::initialize() {
        Entity::initialize();

        audio::setGlobalVolume(GlobalVolume);
        audio::playSound(slurp::Globals->GameAssets->backgroundMusic, 0.5, true);
    }

    void GameGlobal::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (
            (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) ||
            keyboardState.isDown(slurp::KeyboardCode::ESC)
        ) { slurp::Globals->PlatformDll->shutdown(); }
    }

    void GameGlobal::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            slurp::Globals->PlatformDll->shutdown();
        }
    }
}
