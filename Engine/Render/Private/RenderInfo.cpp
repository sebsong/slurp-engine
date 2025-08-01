#include "RenderInfo.h"

#include "Asset.h"

namespace render {
    static slurp::Vec2<float> getRenderOffset(const slurp::Vec2<float>& dimensions, bool isCentered) {
        if (!isCentered) {
            return slurp::Vec2<float>::Zero;
        }
        return -dimensions / 2;
    }

    RenderInfo::RenderInfo()
        : renderingEnabled(false),
          sprite({}),
          renderShape({}),
          renderOffset({}) {}

    RenderInfo::RenderInfo(
        std::string&& spriteFileName,
        bool isCentered
    )
        : renderingEnabled(true),
          sprite(render::loadSprite(spriteFileName)),
          renderShape({}),
          renderOffset(getRenderOffset(sprite.bitmap.dimensions, isCentered)) {}

    RenderInfo::RenderInfo(const Sprite& sprite, bool isCentered)
        : renderingEnabled(true),
          sprite(sprite),
          renderShape({}),
          renderOffset(getRenderOffset(sprite.bitmap.dimensions, isCentered)) {}

    RenderInfo::RenderInfo(
        const RenderShape& renderShape,
        bool isCentered
    ): renderingEnabled(true),
       sprite({}),
       renderShape(renderShape),
       renderOffset(getRenderOffset(renderShape.shape.dimensions, isCentered)) {}

    void RenderInfo::draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& position) const {
        if (!renderingEnabled) { return; }

        slurp::Vec2<float> startPoint = position + renderOffset;
        if (sprite.bitmap.map) { sprite.draw(buffer, startPoint); } else { renderShape.draw(buffer, startPoint); }
    }
}
