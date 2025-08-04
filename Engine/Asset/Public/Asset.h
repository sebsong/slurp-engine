#pragma once

#include "Render.h"
#include "Bitmap.h"
#include "Wave.h"

namespace asset {
    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmapFile(const std::string& bitmapFileName);

    WaveData loadWaveFile(const std::string& waveFileName);
}
