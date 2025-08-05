#pragma once
#include <string>

#include "Wave.h"

namespace audio {
    struct AudioBuffer;

    struct Sound {
        uint32_t sampleIndex;
        uint32_t numSamples;
        audio_sample_t* sampleData;

        void loadAudio(const AudioBuffer& buffer);
    };

    Sound loadSound(const std::string& waveFileName);
}
