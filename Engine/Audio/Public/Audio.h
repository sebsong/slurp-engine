#pragma once
#include <cstdint>

#define NUM_AUDIO_CHANNELS 2
#define AUDIO_BUFFER_SECONDS 1
#define AUDIO_SAMPLES_PER_SECOND 44100
#define AUDIO_WRITE_AHEAD_SECONDS 0.01

namespace audio {
    // typedef int16_t audio_sample_t; // 16-bit Mono
    // typedef int32_t audio_sample_t; // 16-bit Stereo
    typedef int64_t audio_sample_t; // 32-bit stereo

    struct AudioBuffer {
        audio_sample_t* samples;
        int samplesPerSec;
        int numSamplesToWrite;
    };
}
