#pragma once

#include "Platform.h"
#include "DynamicDeclaration.h"
#include "Core.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"

#include "UpdateRenderPipeline.h"

#define MAX_NUM_CONTROLLERS 4
#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 1000

namespace slurp {
    struct Player {
        Entity* entity;
        bool isParryActive;
    };

    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;
        render::ColorPalette colorPalette;

        Entity* mouseCursor;
        Player player;
        Entity* enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        Entity* projectiles[PROJECTILE_POOL_SIZE];
    };

    struct MemorySections {
        UpdateRenderPipeline updateRenderPipeline;
        GameState gameState;
    };

#if DEBUG
    struct RecordingState {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(const platform::PlatformDll platformDll, platform::GameMemory* gameMemory)
#define SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(fnName) void fnName(slurp::MouseState mouseState, slurp::KeyboardState keyboardState, float dt)
#define SLURP_HANDLE_GAMEPAD_INPUT(fnName) void fnName(slurp::GamepadState controllerStates[MAX_NUM_CONTROLLERS], float dt)
#define SLURP_LOAD_AUDIO(fnName) void fnName(slurp::AudioBuffer buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(render::GraphicsBuffer graphicsBuffer, float dt)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT, handleMouseAndKeyboardInput)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_GAMEPAD_INPUT, handleGamepadInput)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_LOAD_AUDIO, loadAudio)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, updateAndRender)

    struct SlurpDll {
        dyn_init* init = stub_init;
        dyn_handleMouseAndKeyboardInput* handleMouseAndKeyboardInput = stub_handleMouseAndKeyboardInput;
        dyn_handleGamepadInput* handleGamepadInput = stub_handleGamepadInput;
        dyn_loadAudio* loadAudio = stub_loadAudio;
        dyn_updateAndRender* updateAndRender = stub_updateAndRender;
    };
}
