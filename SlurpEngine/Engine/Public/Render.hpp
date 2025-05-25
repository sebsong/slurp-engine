#pragma once
#include <Vector.hpp>
#include <cstdint>
#include <array>

#define COLOR_PALETTE_SIZE 8
#define TILEMAP_WIDTH 32
#define TILEMAP_HEIGHT 18

namespace slurp
{
    struct Entity;
    typedef uint8_t ColorPaletteIdx;
    typedef uint32_t Pixel;

    struct GraphicsBuffer
    {
        Pixel* const pixelMap; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
    };

    struct ColorPalette
    {
        Pixel colors[COLOR_PALETTE_SIZE];
    };
    
    struct Tilemap
    {
        std::array<std::array<ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> map;
        uint8_t tileSize;
    };

    void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        float r,
        float g,
        float b
    );

    // TODO: maybe register/save graphics buffer and color palette so we don't need to pass it in every time?
    void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );

    void drawSquare(
        const GraphicsBuffer& buffer,
        Vector2<int> point,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );
    
    void drawLine(
        const GraphicsBuffer& buffer,
        Vector2<int> startPoint,
        Vector2<int> endPoint,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );
    
    void drawEntity(const GraphicsBuffer& buffer, const Entity& entity, const ColorPalette& colorPalette);
    
    void drawBorder(const GraphicsBuffer& buffer, uint8_t borderThickness, uint32_t color);
    
    ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName);
}
