#pragma once

#include "Platform.h"
#include "DynamicDeclaration.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include "Game.h"

namespace slurp {
    struct MemorySections {
        EntityManager entityManager;
        audio::SoundManager soundManager;
        game::GameAssets gameAssets;
        game::GameState gameState;
    };

#if DEBUG
    struct RecordingState {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(platform::GameMemory& gameMemory, const platform::PlatformDll& platformDll, const render::RenderApi& renderApi)
#define SLURP_HANDLE_INPUT(fnName) void fnName(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState, const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS])
#define SLURP_LOAD_AUDIO(fnName) void fnName(const audio::AudioBuffer& buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(float dt)

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

    static const platform::PlatformDll* GlobalPlatformDll;
    static const render::RenderApi* GlobalRenderApi;
    static EntityManager* GlobalEntityManager;
    static audio::SoundManager* GlobalSoundManager;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif
}
