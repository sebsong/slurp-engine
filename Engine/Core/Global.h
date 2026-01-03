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
    struct GameAssets;
    struct Global;
    struct PauseMenu;
    struct MainMenu;
    struct Game;
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
        game::GameAssets* Assets;
        game::Global* GlobalScene;
        game::MainMenu* MainMenuScene;
        game::Game* GameScene;
        game::PauseMenu* PauseMenuScene;
    };

    GLOBAL(Global* Globals)

#if DEBUG
    GLOBAL(RecordingState GlobalRecordingState)
#endif
}
