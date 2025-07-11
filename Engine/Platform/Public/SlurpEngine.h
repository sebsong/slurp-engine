#pragma once

#include "Platform.h"
#include "DynamicDeclaration.h"
#include "Entity.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "EntityManager.h"

#include "Player.h"

#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 10

namespace slurp {
    // TODO: move this to the game layer
    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;
        render::ColorPalette colorPalette;

        Entity background;
        Entity wallUp;
        Entity wallDown;
        Entity wallLeft;
        Entity wallRight;
        Entity obstacle1;
        Entity obstacle2;

        Entity mouseCursor;
        game::Player player;
        Entity enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        Entity projectiles[PROJECTILE_POOL_SIZE];
        Entity colorPaletteSwatch[COLOR_PALETTE_SIZE];
    };

    struct MemorySections {
        EntityManager entityManager;
        GameState gameState;
    };

#if DEBUG
    struct RecordingState {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(const platform::PlatformDll platformDll, platform::GameMemory* gameMemory)
#define SLURP_HANDLE_INPUT(fnName) void fnName(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState, const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS])
#define SLURP_LOAD_AUDIO(fnName) void fnName(const slurp::AudioBuffer& buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(const render::GraphicsBuffer& graphicsBuffer, float dt)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_INPUT, handleInput)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_LOAD_AUDIO, loadAudio)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, updateAndRender)

    struct SlurpDll {
        dyn_init* init = stub_init;
        dyn_handleInput* handleInput = stub_handleInput;
        dyn_loadAudio* loadAudio = stub_loadAudio;
        dyn_updateAndRender* updateAndRender = stub_updateAndRender;
    };
}
