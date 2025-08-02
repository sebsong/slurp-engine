#include "Sprite.h"

namespace render {
    void Sprite::draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& startPoint) const {
        const slurp::Vec2<int> roundedStartPoint = static_cast<slurp::Vec2<int>>(startPoint);
        const slurp::Vec2<float> endPoint = startPoint + bitmap.dimensions;
        const slurp::Vec2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vec2<int> clampedEndPoint = _getClamped(buffer, endPoint);

        for (int y = clampedStartPoint.y; y < clampedEndPoint.y; y++) {
            for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) {
                Pixel pixel = bitmap.map[(y - roundedStartPoint.y) * bitmap.dimensions.x + (x - roundedStartPoint.x)];
                _drawAtPoint(
                    buffer,
                    {x, y},
                    pixel
                );
            }
        }
    }

    Sprite loadSprite(const std::string& spriteFileName) {
        return Sprite{asset::loadBitmapFile(spriteFileName)};
    }
}
