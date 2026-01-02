#pragma once
#include "Recording.h"

namespace memory {
    struct GameMemory;
}

namespace platform {
    struct PlatformDll;
}

namespace render {
    struct RenderApi;
}

namespace timer {
    class Timer;
}

namespace job {
    class JobRunner;
}

namespace asset {
    class AssetLoader;
}

namespace entity {
    class EntityPipeline;
}

namespace audio {
    class AudioPlayer;
}

namespace game {
    struct PauseMenuState;
    struct PauseMenuAssets;
    struct MainMenuState;
    struct MainMenuAssets;
    struct GameState;
    struct GameAssets;
}

#define GLOBAL(decl) \
    inline decl; \
    extern decl;

namespace slurp {
    class EntityPipeline;

    // TODO: maybe have static helper methods instead of needing to reference these directly
    struct Global {
        memory::GameMemory* GameMemory;

        const platform::PlatformDll* PlatformDll;
        const render::RenderApi* RenderApi;

        timer::Timer* Timer;
        job::JobRunner* JobRunner;
        asset::AssetLoader* AssetLoader;
        entity::EntityPipeline* EntityPipeline;
        audio::AudioPlayer* AudioPlayer;

        // TODO: should these live in the game module to give the game module full ownership
        game::MainMenuAssets* MenuAssets;
        game::MainMenuState* MenuState;
        game::GameAssets* GameAssets;
        game::GameState* GameState;
        game::PauseMenuAssets* PauseAssets;
        game::PauseMenuState* PauseState;
    };

    GLOBAL(Global* Globals)

#if DEBUG
    GLOBAL(RecordingState GlobalRecordingState)
#endif
}
