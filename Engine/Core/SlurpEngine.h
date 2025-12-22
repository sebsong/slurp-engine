#pragma once

#include "Platform.h"
#include "Timer.h"
#include "JobRunner.h"
#include "AssetLoader.h"
#include "DynamicDeclaration.h"
#include "Input.h"
#include "Audio.h"
#include "EntityPipeline.h"
#include "AudioPlayer.h"
#include "Game.h"

#if UNITY_BUILD

/** Single translation unit, unity build **/

// TODO: figure out if we can move these with the other unity build includes
// ReSharper disable once CppUnusedIncludeDirective
#include "SpinLock.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryConstructs.cpp"

#endif

struct MIX_Mixer;

namespace slurp {
    struct EngineSystems {
        timer::Timer timer;
        job::JobRunner jobRunner;
        asset::AssetLoader assetLoader;
        entity::EntityPipeline entityPipeline;
        audio::AudioPlayer audioPlayer;
    };

    void init(
        memory::MemoryArena& permanentMemory,
        memory::MemoryArena& transientMemory,
        const platform::PlatformDll& platformDll,
        const render::RenderApi& renderApi,
        MIX_Mixer* audioMixer,
        bool isInitialized
    );

    void frameStart();

    void handleInput(
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    );

    void bufferAudio(const audio::AudioBuffer& buffer);

    void updateAndRender(float dt);

    void frameEnd();

    void shutdown();
}
