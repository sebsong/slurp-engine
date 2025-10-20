#include "SlurpEngine.h"
#include "Random.h"
#include "Debug.h"
#include "Settings.h"

#include <iostream>

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
#include "EntityManager.cpp"


// ReSharper disable once CppUnusedIncludeDirective
#include "PlayingSound.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "SoundManager.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Game.cpp"

namespace slurp {
    SLURP_INIT(init) {
        memory::GlobalGameMemory.permanent = memory::MemoryArena("Permanent", permanentMemory);
        memory::GlobalGameMemory.transient = memory::MemoryArena("Transient", transientMemory);
        memory::GlobalGameMemory.singleFrame =
                memory::GlobalGameMemory.transient.allocateSubArena("Single Frame",SINGLE_FRAME_ARENA_SIZE);
        memory::GlobalGameMemory.assetLoader =
                memory::GlobalGameMemory.transient.allocateSubArena("Asset Loader",ASSET_LOADER_ARENA_SIZE);

        GlobalPlatformDll = &platformDll;
        GlobalRenderApi = &renderApi;

        // TODO: when hot reloading, don't reinitialize these systems
        EngineSystems* engineSystems = memory::GlobalGameMemory.permanent.allocate<EngineSystems>();
        GlobalTimer = new(&engineSystems->timer) timer::Timer();
        GlobalJobRunner = new(&engineSystems->jobRunner) job::JobRunner();
        GlobalAssetLoader = new(&engineSystems->assetLoader) asset::AssetLoader();
        GlobalEntityManager = new(&engineSystems->entityManager) EntityManager();
        GlobalSoundManager = new(&engineSystems->soundManager) audio::SoundManager();
#if DEBUG
        GlobalRecordingState = memory::GlobalGameMemory.transient.allocate<RecordingState>();
#endif

        game::GameAssets* gameAssets = memory::GlobalGameMemory.permanent.allocate<game::GameAssets>();
        game::GameState* gameState = memory::GlobalGameMemory.permanent.allocate<game::GameState>();
        game::initGame(gameAssets, gameState);
        GlobalEntityManager->initialize();
    }

    SLURP_FRAME_START(frameStart) {}

    SLURP_HANDLE_INPUT(handleInput) {
        GlobalEntityManager->handleInput(mouseState, keyboardState, gamepadStates);

#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) { GlobalPlatformDll->DEBUG_togglePause(); }
        if (keyboardState.justPressed(KeyboardCode::R) && !GlobalRecordingState->isPlayingBack) {
            if (!GlobalRecordingState->isRecording) {
                GlobalRecordingState->isRecording = true;
                GlobalPlatformDll->DEBUG_beginRecording();
            } else {
                GlobalPlatformDll->DEBUG_endRecording();
                GlobalRecordingState->isRecording = false;
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T)) {
            GlobalRecordingState->isPlayingBack = true;
            auto onPlaybackEnd = []() -> void { GlobalRecordingState->isPlayingBack = false; };
            GlobalPlatformDll->DEBUG_beginPlayback(onPlaybackEnd);
        }
#endif
    }

    SLURP_BUFFER_AUDIO(bufferAudio) {
        GlobalSoundManager->bufferAudio(buffer);
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        GlobalEntityManager->updateAndRender(dt);

#if DEBUG
        if (GlobalRecordingState->isRecording) {
            debug::drawRectBorder(
                {-CAMERA_WORLD_WIDTH_MAX, -CAMERA_WORLD_HEIGHT_MAX},
                {CAMERA_WORLD_WIDTH_MAX,CAMERA_WORLD_HEIGHT_MAX},
                10,
                DEBUG_RED_COLOR
            );
        } else if (GlobalRecordingState->isPlayingBack) {
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
        memory::GlobalGameMemory.singleFrame.freeAll();
    }
}
