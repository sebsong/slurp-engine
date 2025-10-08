#pragma once
#include <string>

#include "Wave.h"

namespace audio {
    struct AudioBuffer;

    struct Sound {
        uint32_t numSamples;
        StereoAudioSampleContainer* sampleData;
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

        void bufferAudio(const AudioBuffer& buffer, float globalVolumeMultiplier);

        bool operator==(const PlayingSound&) const;
    };

    Sound loadSound(const std::string& waveFileName);
}
