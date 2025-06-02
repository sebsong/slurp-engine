#include "Render.h"
#include "Core.h"
#include "Math.h"
#include "Debug.h"
#include <fstream>
#include <string>

namespace render {
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../assets/";
#endif

    static void _drawAtPoint(const GraphicsBuffer& buffer, const slurp::Vector2<int>& point, Pixel color) {
        *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = color;
    }

    static void _clampToBufferBounds(const GraphicsBuffer& buffer, slurp::Vector2<int>& point) {
        point.x = std::min(std::max(point.x, 0), buffer.widthPixels);
        point.y = std::min(std::max(point.y, 0), buffer.heightPixels);
    }

    static void _drawRect(
        const GraphicsBuffer& buffer,
        slurp::Vector2<int> minPoint,
        slurp::Vector2<int> maxPoint,
        Pixel color
    ) {
        _clampToBufferBounds(buffer, minPoint);
        _clampToBufferBounds(buffer, maxPoint);
        for (int y = minPoint.y; y < maxPoint.y; y++) {
            for (int x = minPoint.x; x < maxPoint.x; x++) {
                _drawAtPoint(buffer, {x, y}, color);
            }
        }
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int> minPoint,
        const slurp::Vector2<int> maxPoint,
        float r,
        float g,
        float b
    ) {
        uint8_t red = math::round(r * 255);
        uint8_t green = math::round(g * 255);
        uint8_t blue = math::round(b * 255);
        Pixel color = (red << 16) | (green << 8) | blue;
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int> minPoint,
        const slurp::Vector2<int> maxPoint,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        Pixel color = colorPalette.colors[colorPaletteIdx];
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    void drawSquare(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int> point,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            colorPaletteIdx,
            colorPalette
        );
    }

    void drawLine(
        const GraphicsBuffer& buffer,
        slurp::Vector2<int> startPoint,
        slurp::Vector2<int> endPoint,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        _clampToBufferBounds(buffer, startPoint);
        _clampToBufferBounds(buffer, endPoint);

        slurp::Vector2<int> startToEnd = endPoint - startPoint;
        slurp::Vector2<float> direction = static_cast<slurp::Vector2<float>>(startToEnd).normalize();

        slurp::Vector2<float> currentPoint = startPoint;
        float distance = startToEnd.magnitude();
        while (distance > 0) {
            drawSquare(
                buffer,
                currentPoint,
                size,
                colorPaletteIdx,
                colorPalette
            );
            currentPoint += direction;
            distance--;
        }
    }

    void drawEntity(const GraphicsBuffer& buffer, const slurp::Entity& entity, const ColorPalette& colorPalette) {
        // NOTE: Sizes that are even will have an off-center position
        slurp::Vector2<int> point = entity.position - entity.positionOffset;
        drawSquare(
            buffer,
            point,
            entity.size,
            entity.color,
            colorPalette
        );
    }

    void drawBorder(const GraphicsBuffer& buffer, uint8_t borderThickness, uint32_t color) {
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

    ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName) {
        ColorPalette palette = {};

        const std::string filePath = AssetsDirectory + paletteHexFileName;
        std::ifstream file(filePath);
        assert(file.is_open());

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE) {
            Pixel color = std::stoi(line, nullptr, 16);
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
    }
}
