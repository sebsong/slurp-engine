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
#include "Wave.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Asset.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "RenderInfo.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Render.cpp"


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

namespace slurp {
    SLURP_INIT(init) {
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
        game::initGame(isInitialized);
        entity::initializeEntities();
    }

    SLURP_FRAME_START(frameStart) {}

    SLURP_HANDLE_INPUT(handleInput) {
        entity::handleInput(mouseState, keyboardState, gamepadStates);

#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) { Globals->PlatformDll->DEBUG_togglePause(); }
        if (keyboardState.justPressed(KeyboardCode::R) && !Globals->RecordingState->isPlayingBack) {
            if (!Globals->RecordingState->isRecording) {
                Globals->RecordingState->isRecording = true;
                Globals->PlatformDll->DEBUG_beginRecording();
            } else {
                Globals->PlatformDll->DEBUG_endRecording();
                Globals->RecordingState->isRecording = false;
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T)) {
            Globals->RecordingState->isPlayingBack = true;
            auto onPlaybackEnd = []() -> void { Globals->RecordingState->isPlayingBack = false; };
            Globals->PlatformDll->DEBUG_beginPlayback(onPlaybackEnd);
        }
#endif
    }

    SLURP_BUFFER_AUDIO(bufferAudio) {
        audio::bufferAudio(buffer);
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        entity::updateAndRender(dt);

#if DEBUG
        if (Globals->RecordingState->isRecording) {
            debug::drawRectBorder(
                {-CAMERA_WORLD_WIDTH_MAX, -CAMERA_WORLD_HEIGHT_MAX},
                {CAMERA_WORLD_WIDTH_MAX,CAMERA_WORLD_HEIGHT_MAX},
                10,
                DEBUG_RED_COLOR
            );
        } else if (Globals->RecordingState->isPlayingBack) {
            debug::drawRectBorder(
                {-CAMERA_WORLD_WIDTH_MAX, -CAMERA_WORLD_HEIGHT_MAX},
                {CAMERA_WORLD_WIDTH_MAX,CAMERA_WORLD_HEIGHT_MAX},
                10,
                DEBUG_GREEN_COLOR
            );
        }
#endif
    }

    SLURP_FRAME_END(frameEnd) {
        memory::SingleFrame->freeAll();
    }

    SLURP_SHUTDOWN(shutdown) {
        job::shutdown();
        memory::Transient->freeAll();
    }
}
