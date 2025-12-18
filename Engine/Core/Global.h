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
    struct MenuState;
    struct MenuAssets;
    struct State;
    struct Assets;
}

namespace slurp {
    class EntityPipeline;

    // TODO: maybe have static helper methods instead of needing to reference these directly
    static struct Global {
        memory::GameMemory* GameMemory;

        const platform::PlatformDll* PlatformDll;
        const render::RenderApi* RenderApi;

        timer::Timer* Timer;
        job::JobRunner* JobRunner;
        asset::AssetLoader* AssetLoader;
        entity::EntityPipeline* EntityPipeline;
        audio::AudioPlayer* AudioPlayer;

        // TODO: should these live in the game module to give the game module full ownership
        game::MenuAssets* MenuAssets;
        game::MenuState* MenuState;
        game::Assets* GameAssets;
        game::State* GameState;
    }* Globals;

#if DEBUG
    static RecordingState GlobalRecordingState;
#endif
}
