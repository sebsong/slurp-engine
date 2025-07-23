#include "RenderInfo.h"

namespace render {
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
          sprite(loadSprite(spriteFileName)),
          renderShape({}),
          renderOffset(isCentered ? -sprite.bitmap.dimensions / 2 : slurp::Vector2<int>::Zero) {}

    RenderInfo::RenderInfo(const Sprite& sprite, bool isCentered)
        : renderingEnabled(true),
          sprite(sprite),
          renderShape({}),
          renderOffset(isCentered ? -sprite.bitmap.dimensions / 2 : slurp::Vector2<int>::Zero) {}

    RenderInfo::RenderInfo(
        const RenderShape& renderShape,
        bool isCentered
    ): renderingEnabled(true),
       sprite({}),
       renderShape(renderShape),
       renderOffset(isCentered ? -renderShape.shape.dimensions / 2 : slurp::Vector2<int>::Zero) {}

    void RenderInfo::draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) const {
        if (!renderingEnabled) { return; }

        slurp::Vector2<int> startPoint = position + renderOffset;
        if (sprite.bitmap.map) { sprite.draw(buffer, startPoint); }
        else { renderShape.draw(buffer, startPoint); }
    }
}