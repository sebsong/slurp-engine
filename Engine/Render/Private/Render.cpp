#include "Render.h"
#include "Entity.h"
#include "Math.h"
#include "Debug.h"
#include <fstream>
#include <string>

namespace render {
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";

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
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        Pixel color
    ) {
        const slurp::Vector2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vector2<int> clampedEndPoint = _getClamped(buffer, endPoint);
        for (int y = clampedStartPoint.y; y < clampedEndPoint.y; y++) {
            for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) { _drawAtPoint(buffer, {x, y}, color); }
        }
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        float r,
        float g,
        float b
    ) {
        const uint8_t red = math::round(r * 255);
        const uint8_t green = math::round(g * 255);
        const uint8_t blue = math::round(b * 255);
        const Pixel color = (red << 16) | (green << 8) | blue;
        _drawRect(buffer, startPoint, endPoint, color);
    }

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    ) {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        const Pixel color = colorPalette.colors[colorPaletteIdx];
        _drawRect(buffer, startPoint, endPoint, color);
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
        const float radius = static_cast<float>(size) / 2;
        const slurp::Vector2<int> sizeOffset = slurp::Vector2<int>::Unit * -math::getHypotenuse(radius, radius / 2);
        const slurp::Vector2<int> offsetStartPoint = startPoint + sizeOffset;
        const slurp::Vector2<int> offsetEndPoint = endPoint + sizeOffset;

        const slurp::Vector2<int> startToEnd = offsetEndPoint - offsetStartPoint;
        const slurp::Vector2<float> direction = static_cast<slurp::Vector2<float>>(startToEnd).normalize();

        slurp::Vector2<float> currentPoint = offsetStartPoint;
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

    void drawRectBorder(
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

    void RenderShape::draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) const {
        const slurp::Vector2<int> startPoint = position + renderOffset;
        const slurp::Vector2<int> endPoint = startPoint + shape.dimensions;
        switch (shape.type) {
            case geometry::Rect: {
                _drawRect(buffer, startPoint, endPoint, color);
                break;
            }
            default: { assert(false); }
        }
    }

    ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName) {
        ColorPalette palette = {};

        const std::string filePath = PalettesDirectory + paletteHexFileName;
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
