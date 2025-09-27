#include "SlurpEngine.h"
#include "Random.h"
#include "Debug.h"

#include <iostream>

//TODO: have option to not do unity build
/* Single translation unit, unity build */
// ReSharper disable once CppUnusedIncludeDirective
#include "Asset.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Audio.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Collision.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Entity.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "EntityManager.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Physics.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Render.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "RenderApi.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "RenderInfo.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Sprite.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Sound.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "SoundManager.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Timer.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Update.cpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "Game.cpp"

namespace slurp {
    SLURP_INIT(init) {
        GlobalPlatformDll = &platformDll;
        GlobalRenderApi = &renderApi;

        ASSERT(sizeof(MemorySections) <= gameMemory.permanentMemory.sizeBytes);
        MemorySections* sections = static_cast<MemorySections*>(gameMemory.permanentMemory.memory);

        new(&sections->entityManager) EntityManager();
        GlobalEntityManager = &sections->entityManager;
        new(&sections->soundManager) audio::SoundManager();
        GlobalSoundManager = &sections->soundManager;
#if DEBUG
        ASSERT(sizeof(RecordingState) <= gameMemory.transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory.transientMemory.memory);
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

    SLURP_LOAD_AUDIO(loadAudio) {
        GlobalSoundManager->loadAudio(buffer);
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        GlobalEntityManager->updateAndRender(graphicsBuffer, dt);

#if DEBUG
        if (GlobalRecordingState->isRecording) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {
                    static_cast<float>(graphicsBuffer.widthPixels),
                    static_cast<float>(graphicsBuffer.heightPixels)
                },
                10,
                DEBUG_RED_COLOR
            );
        } else if (GlobalRecordingState->isPlayingBack) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {
                    static_cast<float>(graphicsBuffer.widthPixels),
                    static_cast<float>(graphicsBuffer.heightPixels)
                },
                10,
                DEBUG_GREEN_COLOR
            );
        }
#endif
    }
}
