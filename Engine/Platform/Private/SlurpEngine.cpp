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
        memory::GlobalGameMemory = &gameMemory;
        GlobalPlatformDll = &platformDll;
        GlobalRenderApi = &renderApi;

        // TODO: when hot reloading, don't reinitialize these systems
        MemorySections* sections = gameMemory.permanent.allocate<MemorySections>();

        new(&sections->engineSystems.jobRunner) job::JobRunner();
        GlobalJobRunner = &sections->engineSystems.jobRunner;
        new(&sections->engineSystems.assetLoader) asset::AssetLoader();
        GlobalAssetLoader = &sections->engineSystems.assetLoader;
        new(&sections->engineSystems.entityManager) EntityManager();
        GlobalEntityManager = &sections->engineSystems.entityManager;
        new(&sections->engineSystems.soundManager) audio::SoundManager();
        GlobalSoundManager = &sections->engineSystems.soundManager;
#if DEBUG
        GlobalRecordingState = gameMemory.transient.allocate<RecordingState>();
#endif

        game::initGame(sections->gameAssets, sections->gameState);
        GlobalEntityManager->initialize();
    }

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
}
