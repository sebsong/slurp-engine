#include "SlurpEngine.h"
#include "Global.h"
#include "Random.h"
#include "Debug.h"
#include "Settings.h"

/** Single translation unit, unity build **/
//TODO: have option to not do unity build

// ReSharper disable once CppUnusedIncludeDirective
#include "Debug.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "JobRunner.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Timer.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Bitmap.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Sprite.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "SpriteAnimation.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Wave.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Asset.cpp"

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
#include "Game.cpp"

#if DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#include "Recording.cpp"
#endif

namespace slurp {
    SLURP_INIT(slurp_init) {
        /** Memory **/
        if (!isInitialized) {
            Globals = permanentMemory.allocate<Global>();
            Globals->GameMemory = permanentMemory.allocate<memory::GameMemory>();
            Globals->GameMemory->permanent = &permanentMemory;
            Globals->GameMemory->transient = &transientMemory;
        } else {
            Globals = reinterpret_cast<Global*>(permanentMemory.getMemoryBlock().memory);
        }
        Globals->GameMemory->singleFrame =
                Globals->GameMemory->transient->allocateSubArena("Single Frame",SINGLE_FRAME_ARENA_SIZE);
        Globals->GameMemory->assetLoader =
                Globals->GameMemory->transient->allocateSubArena("Asset Loader",ASSET_LOADER_ARENA_SIZE);
        memory::Permanent = Globals->GameMemory->permanent;
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
            Globals->AssetLoader = new(&engineSystems->assetLoader) asset::AssetLoader();
            Globals->EntityPipeline = new(&engineSystems->entityPipeline) entity::EntityPipeline();
            Globals->AudioPlayer = new(&engineSystems->audioPlayer) audio::AudioPlayer();
        }
        job::initialize();
#if DEBUG
        Globals->RecordingState = new(memory::Transient->allocate<RecordingState>()) RecordingState();
#endif

        /** Game **/
        game::initialize(isInitialized);
        entity::initializeEntities();
    }

    SLURP_FRAME_START(slurp_frameStart) {}

    SLURP_HANDLE_INPUT(slurp_handleInput) {
        const MouseState* actualMouseState = &mouseState;
        const KeyboardState* actualKeyboardState = &keyboardState;
        const GamepadState (*actualGamepadStates)[MAX_NUM_GAMEPADS] = &gamepadStates;
#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) {
            Globals->PlatformDll->DEBUG_togglePause();
        }
        if (keyboardState.justPressed(KeyboardCode::R) && !Globals->RecordingState->isPlayingBack) {
            if (!Globals->RecordingState->isRecording) {
                beginRecording(Globals->RecordingState);
            } else {
                endRecording(Globals->RecordingState);
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T)) {
            beginPlayback(Globals->RecordingState);
        }

        if (Globals->RecordingState->isRecording) {
            recordInput(Globals->RecordingState, mouseState, keyboardState, gamepadStates);
        }
        MouseState recordingMouseState{};
        KeyboardState recordingKeyboardState{};
        GamepadState (recordingGamepadStates)[MAX_NUM_GAMEPADS]{};
        if (Globals->RecordingState->isPlayingBack) {
            readInputRecording(
                Globals->RecordingState,
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
        entity::handleInput(*actualMouseState, *actualKeyboardState, *actualGamepadStates);
    }

    SLURP_BUFFER_AUDIO(slurp_bufferAudio) {
        audio::bufferAudio(buffer);
    }

    SLURP_UPDATE_AND_RENDER(slurp_updateAndRender) {
        timer::tick(dt);

        game::update(dt);
        entity::updateAndRender(dt);

#if DEBUG
        if (Globals->RecordingState->isRecording) {
            debug::drawRectBorder(
                {-WORLD_WIDTH_MAX, -WORLD_HEIGHT_MAX},
                {WORLD_WIDTH_MAX,WORLD_HEIGHT_MAX},
                10,
                DEBUG_RED_COLOR
            );
        } else if (Globals->RecordingState->isPlayingBack) {
            debug::drawRectBorder(
                {-WORLD_WIDTH_MAX, -WORLD_HEIGHT_MAX},
                {WORLD_WIDTH_MAX,WORLD_HEIGHT_MAX},
                10,
                DEBUG_GREEN_COLOR
            );
        }
#endif
    }

    SLURP_FRAME_END(slurp_frameEnd) {
        memory::SingleFrame->freeAll();
    }

    SLURP_SHUTDOWN(slurp_shutdown) {
        job::shutdown();
        timer::shutdown();
        memory::Transient->freeAll();
    }
}
