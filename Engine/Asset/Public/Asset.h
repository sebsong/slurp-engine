#pragma once

#include "Audio.h"
#include "Render.h"
#include "Bitmap.h"
#include "Wave.h"

namespace asset {
    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Bitmap loadBitmap(const std::string& bitmapFileName);

    audio::AudioData loadAudio(const std::string& filename);
}
