#pragma once

#include "Platform.h"
#include "Timer.h"
#include "JobRunner.h"
#include "Asset.h"
#include "DynamicDeclaration.h"
#include "Input.h"
#include "Audio.h"
#include "EntityPipeline.h"
#include "AudioPlayer.h"
#include "Game.h"

/** Single translation unit, unity build **/
//TODO: have option to not do unity build

// TODO: figure out if we can move these with the other unity build includes
// ReSharper disable once CppUnusedIncludeDirective
#include "SpinLock.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryConstructs.cpp"

namespace slurp {
    struct EngineSystems {
        timer::Timer timer;
        job::JobRunner jobRunner;
        asset::AssetLoader assetLoader;
        entity::EntityPipeline entityPipeline;
        audio::AudioPlayer audioPlayer;
    };

#define SLURP_INIT(fnName) void fnName(memory::MemoryArena& permanentMemory, memory::MemoryArena& transientMemory, const platform::PlatformDll& platformDll, const render::RenderApi& renderApi, bool isInitialized)
#define SLURP_FRAME_START(fnName) void fnName()
#define SLURP_HANDLE_INPUT(fnName) void fnName(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState, const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS])
#define SLURP_BUFFER_AUDIO(fnName) void fnName(const audio::AudioBuffer& buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(float dt)
#define SLURP_FRAME_END(fnName) void fnName()
#define SLURP_SHUTDOWN(fnName) void fnName()

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, slurp_init)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_FRAME_START, slurp_frameStart)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_INPUT, slurp_handleInput)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_BUFFER_AUDIO, slurp_bufferAudio)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, slurp_updateAndRender)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_FRAME_END, slurp_frameEnd)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_SHUTDOWN, slurp_shutdown)

    struct SlurpDll {
        dyn_slurp_init* init = stub_slurp_init;
        dyn_slurp_frameStart* frameStart = stub_slurp_frameStart;
        dyn_slurp_handleInput* handleInput = stub_slurp_handleInput;
        dyn_slurp_bufferAudio* bufferAudio = stub_slurp_bufferAudio;
        dyn_slurp_updateAndRender* updateAndRender = stub_slurp_updateAndRender;
        dyn_slurp_frameEnd* frameEnd = stub_slurp_frameEnd;
        dyn_slurp_shutdown* shutdown = stub_slurp_shutdown;
    };

}
