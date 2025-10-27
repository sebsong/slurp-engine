#include "RenderInfo.h"

namespace render {
    static slurp::Vec2<float> getRenderOffset(const slurp::Vec2<float>& dimensions, bool isCentered) {
        if (!isCentered) {
            return slurp::Vec2<float>::Zero;
        }
        return -dimensions / 2;
    }

    RenderInfo::RenderInfo()
        : renderingEnabled(false),
          sprite(nullptr),
          renderShape({}),
          zOrder(0),
          renderOffset({}) {}

    RenderInfo::RenderInfo(
        const asset::Sprite* sprite,
        bool isCentered,
        int zOrder
    )
        : renderingEnabled(true),
          sprite(sprite),
          renderShape({}),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite->dimensions, isCentered)) {}

    RenderInfo::RenderInfo(
        const asset::Sprite* sprite,
        bool isCentered,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    )
        : renderingEnabled(true),
          sprite(sprite),
          renderShape({}),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite->dimensions, isCentered) + renderOffset) {}

    RenderInfo::RenderInfo(
        const RenderShape& renderShape,
        bool isCentered,
        int zOrder
    ): renderingEnabled(true),
       sprite(nullptr),
       renderShape(renderShape),
       zOrder(zOrder),
       renderOffset(getRenderOffset(renderShape.shape.dimensions, isCentered)) {}

    void RenderInfo::draw(const slurp::Vec2<float>& position) const {
        if (!renderingEnabled) { return; }

        slurp::Vec2<float> startPoint = position + renderOffset;
        if (sprite && !sprite->dimensions.isZero()) {
            sprite->draw(startPoint, zOrder);
        } else {
            // TODO: this path is deprecated
            renderShape.draw(startPoint);
        }
    }
}
