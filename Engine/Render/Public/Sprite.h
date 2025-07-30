#pragma once
#include "Render.h"

#include <cstdint>

namespace render {
    constexpr uint8_t FourBitMaskLow = 0b00001111;
    constexpr uint8_t FourBitMaskHigh = 0b11110000;

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
        Pixel* map;
    };

    struct Sprite {
        Bitmap bitmap;

        void draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& startPoint) const;
    };

    Sprite loadSprite(const std::string& spriteFileName);
}
