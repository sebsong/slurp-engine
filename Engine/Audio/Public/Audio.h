#pragma once
#include <cstdint>

#include "BitTwiddle.h"

#define MONO_NUM_AUDIO_CHANNELS 1
#define STEREO_NUM_AUDIO_CHANNELS 2
#define NUM_AUDIO_CHANNELS STEREO_NUM_AUDIO_CHANNELS
#define IS_MONO_AUDIO (NUM_AUDIO_CHANNELS == MONO_NUM_AUDIO_CHANNELS)
#define IS_STEREO_AUDIO (NUM_AUDIO_CHANNELS == STEREO_NUM_AUDIO_CHANNELS)
#define ONLY_AUDIO_CHANNEL_IDX 0
#define LEFT_AUDIO_CHANNEL_IDX 0
#define RIGHT_AUDIO_CHANNEL_IDX 1

#define TOTAL_AUDIO_SAMPLE_SIZE sizeof(audio::audio_sample_t)
#define PER_CHANNEL_AUDIO_SAMPLE_SIZE (TOTAL_AUDIO_SAMPLE_SIZE / NUM_AUDIO_CHANNELS)
#define PER_CHANNEL_AUDIO_SAMPLE_SIZE_BITS PER_CHANNEL_AUDIO_SAMPLE_SIZE * BITS_PER_BYTE
#define AUDIO_CHANNEL_MASK (~static_cast<uint64_t>(0) >> ((sizeof(uint64_t) - PER_CHANNEL_AUDIO_SAMPLE_SIZE) * BITS_PER_BYTE))

#define AUDIO_BUFFER_SECONDS 1
#if DEBUG
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.1f
#else
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.01f
#endif
#define AUDIO_SAMPLES_PER_SECOND 44100

namespace audio {
    // typedef int16_t audio_sample_t; // 16-bit Mono
    typedef int32_t audio_sample_t; // 16-bit Stereo
    // typedef int64_t audio_sample_t; // 32-bit stereo TODO: move to xaudio2 to allow for 32 bit samples

    struct AudioBuffer {
        audio_sample_t* samples;
        int samplesPerSec;
        int numSamplesToWrite;
    };

    inline audio_sample_t getChannelSample(const audio_sample_t& sample, uint8_t channelIdx) {
        uint8_t channelShift = channelIdx * PER_CHANNEL_AUDIO_SAMPLE_SIZE_BITS;
        return ((AUDIO_CHANNEL_MASK << channelShift) & sample) >> channelShift;
    }
}
