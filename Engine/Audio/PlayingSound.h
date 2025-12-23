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
        uint32_t id;
        const asset::Sound* sound; // NOTE: debug
        std::function<void()> onFinish;
        MIX_Track* audioTrack;
        bool isStopped;

        PlayingSound(
            uint32_t id,
            const asset::Sound* sound,
            const std::function<void()>& onFinish,
            MIX_Track* audioTrack
        );

        bool operator==(const PlayingSound&) const;
    };
}
