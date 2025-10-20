#pragma once

#include "Platform.h"
#include "JobRunner.h"
#include "Asset.h"
#include "DynamicDeclaration.h"
#include "Input.h"
#include "Audio.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include "Game.h"

/** Single translation unit, unity build **/
//TODO: have option to not do unity build

// TODO: figure out if we can move these with the other unity build includes
// ReSharper disable once CppUnusedIncludeDirective
#include "SpinLock.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Memory.cpp"

namespace slurp {
    struct EngineSystems {
        timer::Timer timer;
        job::JobRunner jobRunner;
        asset::AssetLoader assetLoader;
        EntityManager entityManager;
        audio::SoundManager soundManager;
    };

#if DEBUG
    struct RecordingState {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(memory::MemoryBlock& permanentMemory, memory::MemoryBlock& transientMemory, const platform::PlatformDll& platformDll, const render::RenderApi& renderApi)
#define SLURP_FRAME_START(fnName) void fnName()
#define SLURP_HANDLE_INPUT(fnName) void fnName(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState, const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS])
#define SLURP_BUFFER_AUDIO(fnName) void fnName(const audio::AudioBuffer& buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(float dt)
#define SLURP_FRAME_END(fnName) void fnName()

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_FRAME_START, frameStart)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_INPUT, handleInput)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_BUFFER_AUDIO, bufferAudio)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, updateAndRender)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_FRAME_END, frameEnd)

    struct SlurpDll {
        dyn_init* init = stub_init;
        dyn_frameStart* frameStart = stub_frameStart;
        dyn_handleInput* handleInput = stub_handleInput;
        dyn_bufferAudio* bufferAudio = stub_bufferAudio;
        dyn_updateAndRender* updateAndRender = stub_updateAndRender;
        dyn_frameEnd* frameEnd = stub_frameEnd;
    };

    static const platform::PlatformDll* GlobalPlatformDll;
    static const render::RenderApi* GlobalRenderApi;

    static timer::Timer* GlobalTimer;
    static job::JobRunner* GlobalJobRunner;
    static asset::AssetLoader* GlobalAssetLoader;
    static EntityManager* GlobalEntityManager;
    static audio::SoundManager* GlobalSoundManager;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif
}
