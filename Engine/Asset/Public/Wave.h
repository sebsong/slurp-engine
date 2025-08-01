#pragma once
#include <cstdint>

namespace asset {
    // Follows this structure: https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
    struct [[gnu::packed]] WaveFileHeader {
        uint32_t chunkId; // "RIFF"
        uint32_t chunkSize;
        uint32_t waveId; // "WAVE"
    };

    struct [[gnu::packed]] WaveChunkFormat {
        uint32_t formatId; // "fmt "
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
}
