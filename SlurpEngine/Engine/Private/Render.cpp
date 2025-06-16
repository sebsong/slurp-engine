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

    static slurp::Vector2<int> _getClamped(const GraphicsBuffer& buffer, const slurp::Vector2<int>& point) {
        return {
            std::min(std::max(point.x, 0), buffer.widthPixels),
            std::min(std::max(point.y, 0), buffer.heightPixels)
        };
    }

    static void _drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& minPoint,
        const slurp::Vector2<int>& maxPoint,
        Pixel color
    ) {
        const slurp::Vector2<int> clampedMinPoint = _getClamped(buffer, minPoint);
        const slurp::Vector2<int> clampedMaxPoint = _getClamped(buffer, maxPoint);
        for (int y = clampedMinPoint.y; y < clampedMaxPoint.y; y++) {
            for (int x = clampedMinPoint.x; x < clampedMaxPoint.x; x++) {
                _drawAtPoint(buffer, {x, y}, color);
            }
        }
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& minPoint,
        const slurp::Vector2<int>& maxPoint,
        float r,
        float g,
        float b
    ) {
        const uint8_t red = math::round(r * 255);
        const uint8_t green = math::round(g * 255);
        const uint8_t blue = math::round(b * 255);
        const Pixel color = (red << 16) | (green << 8) | blue;
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& minPoint,
        const slurp::Vector2<int>& maxPoint,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        const Pixel color = colorPalette.colors[colorPaletteIdx];
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    static void _drawSquare(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& point,
        int size,
        Pixel color
    ) {
        _drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            color
        );
    }

    void drawSquare(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& point,
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

    static void _drawLine(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        int size,
        Pixel color
    ) {
        const slurp::Vector2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vector2<int> clampedEndPoint = _getClamped(buffer, endPoint);

        const slurp::Vector2<int> startToEnd = clampedEndPoint - startPoint;
        const slurp::Vector2<float> direction = static_cast<slurp::Vector2<float>>(startToEnd).normalize();

        slurp::Vector2<float> currentPoint = clampedStartPoint;
        float distance = startToEnd.magnitude();
        while (distance > 0) {
            _drawSquare(
                buffer,
                currentPoint,
                size,
                color
            );
            currentPoint += direction;
            distance--;
        }
    }

    void drawLine(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        const Pixel color = colorPalette.colors[colorPaletteIdx];
        _drawLine(
            buffer,
            startPoint,
            endPoint,
            size,
            color
        );
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

    void drawBorder(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        const uint8_t borderThickness,
        const Pixel color
    ) {
        _drawLine(
            buffer,
            startPoint,
            {endPoint.x, startPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            {endPoint.x, startPoint.y},
            endPoint,
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            endPoint,
            {startPoint.x, endPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            {startPoint.x, endPoint.y},
            startPoint,
            borderThickness,
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
