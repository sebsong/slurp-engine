#pragma once
#include "Vector.h"
#include "Geometry.h"
#include <cstdint>

#include "Types.h"

#define COLOR_PALETTE_SIZE 8

namespace slurp {
    struct Entity;
}

namespace render {
    typedef uint8_t ColorPaletteIdx;
    typedef uint32_t Pixel;

    struct GraphicsBuffer {
        Pixel* const pixelMap; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
    };

    // TODO: move to game layer
    struct ColorPalette {
        Pixel colors[COLOR_PALETTE_SIZE];
    };

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
        slurp::Vector2<int> dimensions;
        Pixel* map;
    };

    struct Sprite {
        Bitmap bitmap;

        void draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const;
    };

    struct RenderShape {
        geometry::Shape shape;
        Pixel color;

        void draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const;
    };

    template<typename T>
    concept Renderable = requires(T renderable, const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) {
        { renderable.draw(buffer, position) } -> std::same_as<void>;
    };

    template<Renderable T>
    void drawRenderable(const GraphicsBuffer& buffer, const T& renderable, const slurp::Vector2<int>& position) {
        renderable.draw(buffer, position);
    }

    void drawRectBorder(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        uint8_t borderThickness,
        uint32_t color
    );

    ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    Sprite loadSprite(const std::string& spriteFileName);
}
