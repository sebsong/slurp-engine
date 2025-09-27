#pragma once

#include "Wave.h"

namespace asset {
    // TODO: we should stream assets in async
    struct FileReadResult {
        uint32_t sizeBytes;
        types::byte* contents;
    };

    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmapFile(const std::string& bitmapFileName);

    std::string loadVertexShaderSource(const std::string& shaderSourceFileName);

    std::string loadFragmentShaderSource(const std::string& shaderSourceFileName);

    WaveData loadWaveFile(const std::string& waveFileName);
}
