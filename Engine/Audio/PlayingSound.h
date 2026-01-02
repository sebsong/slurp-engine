#pragma once
#include "Audio.h"

#include <functional>

namespace asset {
    struct Sound;
}

struct MIX_Track;

namespace audio {
    struct AudioBuffer;

    struct PlayingSound {
        const asset::Sound* sound;
        std::function<void()> onFinish;
        MIX_Track* audioTrack;
        bool isPaused;
        bool isStopped;

        PlayingSound(
            const asset::Sound* sound,
            const std::function<void()>& onFinish,
            MIX_Track* audioTrack
        );
    };
}
