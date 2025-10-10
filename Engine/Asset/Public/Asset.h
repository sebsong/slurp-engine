#pragma once
#include "Types.h"
#include "Wave.h"

namespace render {
    struct ColorPalette;
    struct Sprite;
}

namespace audio {
    struct Sound;
}

namespace asset {
    struct Bitmap;

    // TODO: we should stream assets in async
    struct FileReadResult {
        uint32_t sizeBytes;
        types::byte* contents;
    };

    struct Asset {
        uint32_t id;
        bool isLoaded;
        // TODO: ref count
    };

    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmapFile(const std::string& bitmapFileName);

    std::string loadVertexShaderSource(const std::string& shaderSourceFileName);

    std::string loadFragmentShaderSource(const std::string& shaderSourceFileName);

    WaveData loadWaveFile(const std::string& waveFileName);
}
