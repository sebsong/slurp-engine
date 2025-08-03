#pragma once
#include <cstdint>

namespace asset {
    // Follows this structure: https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
    struct [[gnu::packed]] RiffChunk{
        char chunkId[4]; // "RIFF"
        uint32_t chunkSize;
        char waveId[4]; // "WAVE"
    };

    struct [[gnu::packed]] FormatChunk {
        char formatId[4]; // "fmt "
        uint32_t chunkSize;
        uint16_t formatTag;
        uint16_t numChannels;
        uint32_t samplesPerSec;
        uint32_t avgBytesPerSec;
        uint16_t blockSize;
        uint16_t bitsPerSample;
        uint16_t cbSize;
        uint16_t validBitsPerSample;
        uint32_t channelMask;
        uint8_t subFormat[16];
    };

    struct [[gnu::packed]] WaveFileHeader {
        RiffChunk riffChunk;
        FormatChunk formatChunk;
    };

    struct [[gnu::packed]] WaveDataChunkHeader {
        char chunkId[4]; // "data"
        uint32_t chunkSize;
    };

    struct WaveFile {};
}
