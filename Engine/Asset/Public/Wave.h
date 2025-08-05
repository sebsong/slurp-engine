#pragma once
#include <cstdint>

#include "Audio.h"
#include "Types.h"

namespace asset {
    // NOTE: Follows this structure: http://soundfile.sapp.org/doc/WaveFormat/
    // NOTE: Designed around Bfxr
    struct [[gnu::packed]] RiffChunk {
        char chunkId[4]; // "RIFF"
        uint32_t chunkSizeBytes;
        char waveId[4]; // "WAVE"
    };

    struct [[gnu::packed]] FormatChunk {
        char formatId[4]; // "fmt "
        uint32_t chunkSizeBytes;
        uint16_t formatTag;
        uint16_t numChannels;
        uint32_t samplesPerSec;
        uint32_t avgBytesPerSec;
        uint16_t blockSizeBytes;
        uint16_t bitsPerSample;
    };

    struct [[gnu::packed]] DataChunkHeader {
        char chunkId[4]; // "data"
        uint32_t chunkSizeBytes;
    };

    struct [[gnu::packed]] WaveChunks {
        RiffChunk riffChunk;
        FormatChunk formatChunk;
        DataChunkHeader dataChunkHeader;
        slurp::byte data[];
    };

    struct WaveData {
        uint32_t numSamples;
        audio::audio_sample_t* sampleData;
    };
}
