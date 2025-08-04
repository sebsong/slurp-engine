#pragma once
#include <string>

#include "Wave.h"

namespace audio {
    struct AudioBuffer;

    struct Sound {
        asset::WaveData waveData;

        void loadAudio(const AudioBuffer& buffer) const;
    };

    Sound loadSound(const std::string& waveFileName);
}
