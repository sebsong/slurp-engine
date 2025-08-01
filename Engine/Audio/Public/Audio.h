#pragma once
#include <cstdint>

namespace audio {
    struct AudioBuffer {
        int32_t* samples; // 16-bit Stereo L + R samples
        int samplesPerSec;
        int samplesToWrite;
    };

    struct AudioData {};
}
