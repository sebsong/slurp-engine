#include <Render.hpp>
#include <Debug.hpp>
#include <fstream>
#include <string>

namespace slurp
{
    static const std::string AssetsDirectory = "../assets/";

    static void drawAtPoint(GraphicsBuffer buffer, Vector2<int> point, Pixel color)
    {
        *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = color;
    }

    static void _drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        Pixel color
    )
    {
        int minX = std::max(minPoint.x, 0);
        int maxX = std::min(maxPoint.x, buffer.widthPixels);
        int minY = std::max(minPoint.y, 0);
        int maxY = std::min(maxPoint.y, buffer.heightPixels);
        for (int y = minY; y < maxY; y++)
        {
            for (int x = minX; x < maxX; x++)
            {
                drawAtPoint(buffer, {x, y}, color);
            }
        }
    }

    static uint8_t round(float num)
    {
        return static_cast<uint8_t>(num + 0.5f);
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        float r,
        float g,
        float b
    )
    {
        uint8_t red = round(r * 255);
        uint8_t green = round(g * 255);
        uint8_t blue = round(b * 255);
        Pixel color = (red << 16) | (green << 8) | blue;
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        ColorPaletteIdx colorPaletteIdx,
        ColorPalette colorPalette
    )
    {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        Pixel color = colorPalette.colors[colorPaletteIdx];
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    void drawSquare(
        const GraphicsBuffer& buffer,
        Vector2<int> point,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        ColorPalette colorPalette
    )
    {
        drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            colorPaletteIdx,
            colorPalette
        );
    }

    void drawBorder(const GraphicsBuffer& buffer, uint8_t borderThickness, uint32_t color)
    {
        _drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, borderThickness},
            color
        );
        _drawRect(
            buffer,
            {0, 0},
            {borderThickness, buffer.heightPixels},
            color
        );
        _drawRect(
            buffer,
            {buffer.widthPixels - borderThickness, 0},
            {buffer.widthPixels, buffer.heightPixels},
            color
        );
        _drawRect(
            buffer,
            {0, buffer.heightPixels - borderThickness},
            {buffer.widthPixels, buffer.heightPixels},
            color
        );
    }
    
    ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName)
    {
        ColorPalette palette = {};

        const std::string filePath = AssetsDirectory + paletteHexFileName;
        std::ifstream file(filePath);

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE)
        {
            Pixel color = std::stoi(line, nullptr, 16);
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
    }
}
