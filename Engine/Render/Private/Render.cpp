#include "Render.h"

#include "Debug.h"

namespace render {
    static void _drawLine(
        const slurp::Vec2<float>& startPoint,
        const slurp::Vec2<float>& endPoint,
        float size,
        Pixel color
    ) {
        // TODO: convert to use graphics api
    }

    void drawRectBorder(
        const slurp::Vec2<float>& startPoint,
        const slurp::Vec2<float>& endPoint,
        const uint8_t borderThickness,
        const Pixel color
    ) {
        _drawLine(
            startPoint,
            {endPoint.x, startPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            {endPoint.x, startPoint.y},
            endPoint,
            borderThickness,
            color
        );
        _drawLine(
            endPoint,
            {startPoint.x, endPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            {startPoint.x, endPoint.y},
            startPoint,
            borderThickness,
            color
        );
    }

    void RenderShape::draw(const slurp::Vec2<float>& startPoint) const {
        // TODO: switch over to using the gpu render api
        // const slurp::Vec2<float> endPoint = startPoint + shape.dimensions;
        switch (shape.type) {
            case geometry::Rect: {
                // _drawRect(buffer, startPoint, endPoint, color);
                break;
            }
            default: { ASSERT(false); }
        }
    }

    Pixel withAlpha(Pixel color, float alpha) {
        ASSERT(alpha >= 0 && alpha <= 1);
        Pixel newAlpha = static_cast<Pixel>(alpha * 255) << AlphaShift;
        return (color & ~AlphaMask) | newAlpha;
    }
}
