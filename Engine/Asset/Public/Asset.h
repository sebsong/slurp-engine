#pragma once

#include "Render.h"
#include "Bitmap.h"
#include "Wave.h"

namespace asset {
    // TODO: we should stream assets in async

    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmapFile(const std::string& bitmapFileName);

    WaveData loadWaveFile(const std::string& waveFileName);
}
