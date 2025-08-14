#pragma once
#include <cstdint>

#define NUM_AUDIO_CHANNELS 2
#define AUDIO_BUFFER_SECONDS 1
#if DEBUG
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.1f
#else
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.01f
#endif
#define AUDIO_SAMPLES_PER_SECOND 44100
#define TOTAL_AUDIO_SAMPLE_SIZE sizeof(audio::audio_sample_t)
#define PER_CHANNEL_AUDIO_SAMPLE_SIZE (TOTAL_AUDIO_SAMPLE_SIZE / NUM_AUDIO_CHANNELS)

namespace audio {
    // typedef int16_t audio_sample_t; // 16-bit Mono
    typedef int32_t audio_sample_t; // 16-bit Stereo
    // typedef int64_t audio_sample_t; // 32-bit stereo TODO: move to xaudio2 to allow for 32 bit samples

    struct AudioBuffer {
        audio_sample_t* samples;
        int samplesPerSec;
        int numSamplesToWrite;
    };
}
