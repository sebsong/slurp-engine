#pragma once
#include "Audio.h"

#include <functional>

namespace asset {
    struct Sound;
}

namespace audio {
    struct AudioBuffer;

    struct PlayingSound {
        uint32_t id;
        const asset::Sound* sound;
        float volumeMultiplier;
        bool shouldLoop;
        std::function<void()> onFinish; // TODO: move sound?
        uint32_t sampleIndex;
        bool isPlaying;

        PlayingSound(
            uint32_t id,
            const asset::Sound* sound,
            float volumeMultiplier,
            bool shouldLoop,
            std::function<void()> onFinish
        );

        void bufferAudio(
            StereoAudioSampleContainer* sampleContainers,
            int numSamplesToWrite,
            float globalVolumeMultiplier,
            bool dampMix
        );

        bool operator==(const PlayingSound&) const;
    };
}
