#pragma once
#include <cstdint>

#include "Audio.h"

namespace asset {
    // NOTE: follows this structure:
    // https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-storage
    struct [[gnu::packed]] BitmapFileHeader {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    };

    struct [[gnu::packed]] BitmapInfoHeader {
        uint32_t biSize;
        uint32_t biWidth;
        uint32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        uint32_t biXPelsPerMeter;
        uint32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    struct [[gnu::packed]] BitmapHeader {
        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;
    };

    struct Bitmap {
        slurp::Vec2<int> dimensions;
        render::Pixel* map;
    };

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

    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmap(const std::string& bitmapFileName);

    audio::AudioData loadAudio(const std::string& filename);
}
