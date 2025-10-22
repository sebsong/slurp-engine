#pragma once

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
    struct GameState;
    struct GameAssets;
}

namespace slurp {
    struct RecordingState;
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
        audio::AudioPlayer* AudioManager;
#if DEBUG
        RecordingState* RecordingState;
#endif

        // TODO: should these live in the game module to give the game module full ownership
        game::GameAssets* GameAssets;
        game::GameState* GameState;
    }* Globals;
}
