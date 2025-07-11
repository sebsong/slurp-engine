#include "SlurpEngine.h"
#include "Random.h"
#include "Debug.h"

#include <iostream>

/* Single translation unit, unity build */
// ReSharper disable once CppUnusedIncludeDirective
#include "Entity.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Update.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Collision.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Render.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "EntityManager.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Timer.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Game.cpp"

typedef unsigned char byte;

namespace slurp {
    static platform::PlatformDll GlobalPlatformDll;
    static GameState* GlobalGameState;
    static EntityManager* GlobalEntityManager;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static constexpr render::ColorPaletteIdx PlayerColorPalletIdx = 3;
    static constexpr render::ColorPaletteIdx PlayerParryColorPalletIdx = 0;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;

    static constexpr float ParryActiveDuration = .1f;

    SLURP_INIT(init) {
        GlobalPlatformDll = platformDll;

        assert(sizeof(MemorySections) <= gameMemory->permanentMemory.sizeBytes);
        MemorySections* sections = static_cast<MemorySections*>(gameMemory->permanentMemory.memory);

        render::ColorPalette colorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        new(&sections->entityManager) EntityManager();
        GlobalEntityManager = &sections->entityManager;

        GlobalGameState = &sections->gameState;
        GlobalGameState->colorPalette = colorPalette;
        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));

        if (!GlobalGameState->isInitialized) {
            // NOTE: anything that shouldn't be hot reloaded goes here
            GlobalGameState->isInitialized = true;
        }

        random::setRandomSeed(GlobalGameState->randomSeed);

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
        game::initGame(sections->gameState, sections->entityManager);
    }

    static void activateParry(game::Player& player) {
        player.isParryActive = true;
        player.renderShape.color = GlobalGameState->colorPalette.colors[PlayerParryColorPalletIdx];
    }

    static void deactivateParry(game::Player& player) {
        player.isParryActive = false;
        player.renderShape.color = GlobalGameState->colorPalette.colors[PlayerColorPalletIdx];
    }

    SLURP_HANDLE_INPUT(handleInput) {
        GlobalEntityManager->handleInput(mouseState, keyboardState, gamepadStates);

        // TODO: move input handling to the game layer
        GlobalGameState->mouseCursor.position = mouseState.position;

        if (mouseState.justPressed(MouseCode::LeftClick)) {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.position;
            projectile.direction =
                    static_cast<Vector2<float>>(mouseState.position - GlobalGameState->player.position).
                    normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) {
                GlobalGameState->projectileIdx = 0;
            }
        }

        if (mouseState.justPressed(MouseCode::RightClick) || keyboardState.justPressed(KeyboardCode::E)) {
            activateParry(GlobalGameState->player);
            timer::delay(ParryActiveDuration, [] { deactivateParry(GlobalGameState->player); });
        }

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4)) {
            GlobalPlatformDll.shutdown();
        }

#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) {
            GlobalPlatformDll.DEBUG_togglePause();
        }
        if (keyboardState.justPressed(KeyboardCode::R) && !GlobalRecordingState->isPlayingBack) {
            if (!GlobalRecordingState->isRecording) {
                GlobalRecordingState->isRecording = true;
                GlobalPlatformDll.DEBUG_beginRecording();
            } else {
                GlobalPlatformDll.DEBUG_endRecording();
                GlobalRecordingState->isRecording = false;
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T)) {
            GlobalRecordingState->isPlayingBack = true;
            auto onPlaybackEnd = []() -> void { GlobalRecordingState->isPlayingBack = false; };
            GlobalPlatformDll.DEBUG_beginPlayback(onPlaybackEnd);
        }
#endif

        if (keyboardState.isDown(KeyboardCode::ESC)) {
            GlobalPlatformDll.shutdown();
        }
        for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
            GamepadState gamepadState = gamepadStates[gamepadIndex];
            if (!gamepadState.isConnected) {
                continue;
            }

            if (gamepadState.isDown(GamepadCode::START) || gamepadState.isDown(GamepadCode::B)) {
                GlobalPlatformDll.shutdown();
            }

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateGamepad(gamepadIndex, leftTrigger, rightTrigger);
        }
    }

    SLURP_LOAD_AUDIO(loadAudio) {}

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        GlobalEntityManager->updateAndRender(graphicsBuffer, dt);

#if DEBUG
        if (GlobalRecordingState->isRecording) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {graphicsBuffer.widthPixels, graphicsBuffer.heightPixels},
                10,
                0x00FF0000
            );
        } else if (GlobalRecordingState->isPlayingBack) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {graphicsBuffer.widthPixels, graphicsBuffer.heightPixels},
                10,
                0x0000FF00
            );
        }
#endif
    }
}
