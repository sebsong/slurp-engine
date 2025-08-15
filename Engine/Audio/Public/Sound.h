#pragma once
#include <string>

#include "Wave.h"

namespace audio {
    struct AudioBuffer;

    struct Sound {
        uint32_t numSamples;
        audio_sample_t* sampleData;
    };

    struct PlayingSound {
        uint32_t id;
        const Sound* sound;
        float volumeMultiplier;
        bool shouldLoop;
        uint32_t sampleIndex;
        bool isPlaying;

        PlayingSound(
            uint32_t id,
            const Sound* sound,
            float volumeMultiplier,
            bool shouldLoop
        );

        void loadAudio(const AudioBuffer& buffer, float globalVolumeMultiplier);

        bool operator==(const PlayingSound&) const;
    };

    Sound loadSound(const std::string& waveFileName);
}
