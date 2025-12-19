#pragma once
#include <cstdint>

#include "Types.h"
#include "Asset.h"

#define WAVE_CHUNK_ID(char1, char2, char3, char4) ( (char4) << 24 | (char3) << 16 | (char2) << 8 | (char1) )

namespace asset {
    enum WaveChunkId {
        Riff = WAVE_CHUNK_ID('R', 'I', 'F', 'F'),
        Wave = WAVE_CHUNK_ID('W', 'A', 'V', 'E'),
        Format = WAVE_CHUNK_ID('f', 'm', 't', ' '),
        Data = WAVE_CHUNK_ID('d', 'a', 't', 'a'),
        Bext = WAVE_CHUNK_ID('b', 'e', 'x', 't'), // https://en.wikipedia.org/wiki/Broadcast_Wave_Format
        Junk = WAVE_CHUNK_ID('j', 'u', 'n', 'k'),
        JUNK = WAVE_CHUNK_ID('J', 'U', 'N', 'K'),
    };

    // NOTE: Follows this structure: http://soundfile.sapp.org/doc/WaveFormat/
    // NOTE: Designed around Bfxr
    struct [[gnu::packed]] RiffChunk {
        uint32_t chunkId; // "RIFF"
        uint32_t chunkSizeBytes;
        uint32_t waveId; // "WAVE"
        types::byte chunkData[];
    };

    struct [[gnu::packed]] FormatChunk {
        uint32_t formatId; // "fmt "
        uint32_t chunkSizeBytes;
        uint16_t formatTag;
        uint16_t numChannels;
        uint32_t samplesPerSec;
        uint32_t avgBytesPerSec;
        uint16_t sampleSizeBytes;
        uint16_t bitsPerChannelSample;
    };

    struct [[gnu::packed]] WaveChunk {
        uint32_t chunkId;
        uint32_t chunkSizeBytes;
        types::byte chunkData[];
    };

    void loadWaveData(Sound* sound, const types::byte* waveFileBytes, uint32_t waveFileSizeBytes);
}
