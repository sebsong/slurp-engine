#include "Render.h"

#include "Entity.h"
#include "Math.h"
#include "Debug.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace render {
    static void _blendAlpha(Pixel& existingColor, const Pixel& newColor, uint8_t alpha) {
        uint8_t existingRed = (existingColor & RedMask) >> RedShift;
        uint8_t existingGreen = (existingColor & GreenMask) >> GreenShift;
        uint8_t existingBlue = (existingColor & BlueMask) >> BlueShift;

        uint8_t newRed = (newColor & RedMask) >> RedShift;
        uint8_t newGreen = (newColor & GreenMask) >> GreenShift;
        uint8_t newBlue = (newColor & BlueMask) >> BlueShift;

        float normalizedAlpha = static_cast<float>(alpha) / 255;
        uint8_t blendedRed = normalizedAlpha * (newRed - existingRed) + existingRed;
        uint8_t blendedGreen = normalizedAlpha * (newGreen - existingGreen) + existingGreen;
        uint8_t blendedBlue = normalizedAlpha * (newBlue - existingBlue) + existingBlue;

        existingColor = (blendedRed << RedShift) | (blendedGreen << GreenShift) | blendedBlue;
    }

    static void _drawAtPoint(const GraphicsBuffer& buffer, const slurp::Vector2<int>& point, Pixel color) {
        uint8_t alpha = color >> AlphaShift;
        if (alpha == 0) {
            return;
        } else if (alpha == 255) {
            *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = color;
        } else {
            Pixel& existingColor = buffer.pixelMap[(point.y * buffer.widthPixels) + point.x];
            _blendAlpha(existingColor, color, alpha);
        }
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
            for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) {
                _drawAtPoint(buffer, {x, y}, color);
            }
        }
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

    void RenderShape::draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const {
        const slurp::Vector2<int> endPoint = startPoint + shape.dimensions;
        switch (shape.type) {
            case geometry::Rect: {
                _drawRect(buffer, startPoint, endPoint, color);
                break;
            }
            default: { assert(false); }
        }
    }

    ColorPalette loadColorPalette(const std::string& paletteHexFileName) {
        ColorPalette palette = {};

        const std::string filePath = PalettesDirectory + paletteHexFileName;
        std::ifstream file(filePath);
        assert(file.good());

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE) {
            Pixel color = std::stoi(line, nullptr, 16);
            if (colorPaletteIdx != 0) {
                color |= AlphaMask;
            }
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
    }

    Pixel withAlpha(Pixel color, float alpha) {
        assert(alpha >= 0 && alpha <= 1);
        Pixel newAlpha = static_cast<Pixel>(alpha * 255) << AlphaShift;
        return (color & ~AlphaMask) | newAlpha;
    }
}
