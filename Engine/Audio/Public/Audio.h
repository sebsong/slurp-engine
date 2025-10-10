#pragma once
#include <cstdint>

#include "BitTwiddle.h"

#define MONO_NUM_AUDIO_CHANNELS 1
#define STEREO_NUM_AUDIO_CHANNELS 2
#define NUM_AUDIO_CHANNELS STEREO_NUM_AUDIO_CHANNELS
#define IS_MONO_AUDIO (NUM_AUDIO_CHANNELS == MONO_NUM_AUDIO_CHANNELS)
#define IS_STEREO_AUDIO (NUM_AUDIO_CHANNELS == STEREO_NUM_AUDIO_CHANNELS)
#define MONO_AUDIO_CHANNEL_IDX 0
#define STEREO_LEFT_AUDIO_CHANNEL_IDX 0
#define STEREO_RIGHT_AUDIO_CHANNEL_IDX 1

#define TOTAL_AUDIO_SAMPLE_SIZE sizeof(audio::audio_sample_t)
#define PER_CHANNEL_AUDIO_SAMPLE_SIZE sizeof(audio::channel_audio_sample_t)
#define PER_CHANNEL_AUDIO_SAMPLE_SIZE_BITS PER_CHANNEL_AUDIO_SAMPLE_SIZE * BITS_PER_BYTE

#define AUDIO_BUFFER_SECONDS 1
#if DEBUG
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.1f
#else
#define AUDIO_BUFFER_WRITE_AHEAD_SECONDS 0.01f
#endif
#define AUDIO_SAMPLES_PER_SECOND 44100

#define MAX_NUM_PLAYING_ONE_SHOT_SOUNDS 10
#define SOUND_DAMP_FACTOR 0.5f

namespace audio {
    // TODO: might be easier to have a separate channel sample type and audio sample type
    // TODO: should have a larger sized type for mixing and a smaller type for the actual buffer
    // typedef int16_t audio_sample_t; // 16-bit Mono
    // typedef int64_t audio_sample_t; // 32-bit stereo TODO: move to xaudio2 to allow for 32 bit samples

    /** Audio sample types  **/
    typedef int32_t audio_sample_t; // 16-bit Stereo
    typedef int16_t channel_audio_sample_t;
    typedef slurp::Vec2<channel_audio_sample_t> StereoAudioSample;

    /** Larger container types for audio mixing **/
    typedef int64_t container_audio_sample_t; // 32-bit stereo
    typedef int32_t channel_audio_sample_container_t;
    typedef slurp::Vec2<channel_audio_sample_container_t> StereoAudioSampleContainer;

    struct AudioBuffer {
        StereoAudioSample* samples;
        int samplesPerSec;
        int numSamplesToWrite;
    };
}
