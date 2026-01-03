#include "SlurpEngine.h"
#include "Global.h"
#include "Game.h"
#include "Random.h"
#include "Debug.h"
#include "Recording.h"
#include "Scene.h"
#include "Settings.h"

#if UNITY_BUILD
/** Single translation unit, unity build **/

// ReSharper disable once CppUnusedIncludeDirective
#include "Debug.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "JobRunner.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Timer.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Bitmap.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "SpriteInstance.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "SpriteAnimation.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "AssetLoader.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Render.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "RenderInfo.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Physics.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Collision.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Update.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Entity.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "EntityPipeline.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "PlayingSound.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "AudioPlayer.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Scene.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Game.cpp"

#if DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#include "Recording.cpp"
#endif

#endif

namespace slurp {
    void init(
        memory::MemoryArena& permanentMemory,
        memory::MemoryArena& transientMemory,
        const platform::PlatformDll& platformDll,
        const render::RenderApi& renderApi,
        MIX_Mixer* audioMixer,
        bool isInitialized
    ) {
        /** Memory **/
        if (!isInitialized) {
            Globals = permanentMemory.allocate<Global>();
            Globals->GameMemory = permanentMemory.allocate<memory::GameMemory>();
            Globals->GameMemory->permanent = &permanentMemory;
            Globals->GameMemory->transient = &transientMemory;
        } else {
            Globals = reinterpret_cast<Global*>(permanentMemory.getMemoryBlock().memory);
        }
        Globals->GameMemory->scene = Globals->GameMemory->permanent->allocateSubArena("Scene",SCENE_ARENA_SIZE);
        Globals->GameMemory->singleFrame =
                Globals->GameMemory->transient->allocateSubArena("Single Frame",SINGLE_FRAME_ARENA_SIZE);
        Globals->GameMemory->assetLoader =
                Globals->GameMemory->transient->allocateSubArena("Asset Loader",ASSET_LOADER_ARENA_SIZE);
        memory::Permanent = Globals->GameMemory->permanent;
        memory::Scene = &Globals->GameMemory->scene;
        memory::Transient = Globals->GameMemory->transient;
        memory::SingleFrame = &Globals->GameMemory->singleFrame;
        memory::AssetLoader = &Globals->GameMemory->assetLoader;

        /** Engine Systems **/
        if (!isInitialized) {
            EngineSystems* engineSystems = memory::Permanent->allocate<EngineSystems>();

            Globals->PlatformDll = &platformDll;
            Globals->RenderApi = &renderApi;
            Globals->Timer = new(&engineSystems->timer) timer::Timer();
            Globals->JobRunner = new(&engineSystems->jobRunner) job::JobRunner();
            Globals->AssetLoader = new(&engineSystems->assetLoader) asset::AssetLoader(audioMixer);
            Globals->EntityPipeline = new(&engineSystems->entityPipeline) entity::EntityPipeline();
            Globals->AudioPlayer = new(&engineSystems->audioPlayer) audio::AudioPlayer(audioMixer);
        }

        /** Game **/
        game::initialize(isInitialized);
    }

    void frameStart() {}

    void handleInput(
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    ) {
        const MouseState* actualMouseState = &mouseState;
        const KeyboardState* actualKeyboardState = &keyboardState;
        const GamepadState (*actualGamepadStates)[MAX_NUM_GAMEPADS] = &gamepadStates;
#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) {
            Globals->PlatformDll->DEBUG_togglePause();
        }
        if (keyboardState.justPressed(KeyboardCode::I) && !GlobalRecordingState.isPlayingBack) {
            if (!GlobalRecordingState.isRecording) {
                beginRecording(GlobalRecordingState);
            } else {
                endRecording(GlobalRecordingState);
            }
        }
        if (keyboardState.justPressed(KeyboardCode::O)) {
            beginPlayback(GlobalRecordingState);
        }

        if (GlobalRecordingState.isRecording) {
            recordInput(GlobalRecordingState, mouseState, keyboardState, gamepadStates);
        }
        MouseState recordingMouseState{};
        KeyboardState recordingKeyboardState{};
        GamepadState recordingGamepadStates[MAX_NUM_GAMEPADS]{};
        if (GlobalRecordingState.isPlayingBack) {
            readInputRecording(
                GlobalRecordingState,
                recordingMouseState,
                recordingKeyboardState,
                recordingGamepadStates
            );
            actualMouseState = &recordingMouseState;
            actualKeyboardState = &recordingKeyboardState;
            actualGamepadStates = &recordingGamepadStates;
        }
#endif

        game::handleMouseAndKeyboardInput(*actualMouseState, *actualKeyboardState);
        for (uint8_t gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
            if (!gamepadStates[gamepadIndex].isConnected) { continue; }
            game::handleGamepadInput(gamepadIndex, *actualGamepadStates[gamepadIndex]);
        }
        scene::handleInput(*actualMouseState, *actualKeyboardState, *actualGamepadStates);
    }

    void bufferAudio(const audio::AudioBuffer& buffer) {}

    void updateAndRender(float dt) {
        timer::tick(dt);

        game::update(dt);
        scene::updateAndRenderEntities(dt);
        scene::updateScenes();

#if DEBUG
        if (GlobalRecordingState.isRecording) {
            debug::drawRectBorder(
                {-WORLD_WIDTH_MAX, -WORLD_HEIGHT_MAX},
                {WORLD_WIDTH_MAX,WORLD_HEIGHT_MAX},
                10,
                DEBUG_RED_COLOR
            );
        } else if (GlobalRecordingState.isPlayingBack) {
            debug::drawRectBorder(
                {-WORLD_WIDTH_MAX, -WORLD_HEIGHT_MAX},
                {WORLD_WIDTH_MAX,WORLD_HEIGHT_MAX},
                10,
                DEBUG_GREEN_COLOR
            );
        }
#endif
    }

    void frameEnd() {
        game::frameEnd();
        memory::SingleFrame->freeAll();
    }

    void shutdown() {
        job::shutdown();
        timer::shutdown();
        memory::Transient->freeAll();
    }
}
