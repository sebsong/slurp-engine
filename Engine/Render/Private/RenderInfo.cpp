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
          zOrder(0),
          renderOffset({}) {}

    RenderInfo::RenderInfo(
        const asset::Sprite* sprite,
        bool isCentered,
        int zOrder
    )
        : renderingEnabled(true),
          sprite(sprite),
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
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite->dimensions, isCentered) + renderOffset) {}

    void draw(const RenderInfo& renderInfo, const slurp::Vec2<float>& position) {
        if (!renderInfo.renderingEnabled) { return; }

        slurp::Vec2<float> startPoint = position + renderInfo.renderOffset;
        const asset::Sprite* sprite = renderInfo.sprite;
        if (sprite && !sprite->dimensions.isZero()) {
            sprite->draw(startPoint, renderInfo.zOrder);
        }
    }
}
