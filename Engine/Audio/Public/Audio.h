#pragma once
#include <cstdint>

namespace audio {
    // typedef int32_t audio_sample_t; // 16-bit Stereo L + R samples
    typedef int16_t audio_sample_t; // 16-bit Mono

    struct AudioBuffer {
        audio_sample_t* samples;
        int samplesPerSec;
        int samplesToWrite;
    };

    struct AudioData {};
}
