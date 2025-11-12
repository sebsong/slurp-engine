#include "RenderInfo.h"

namespace render {
    static slurp::Vec2<float> getRenderOffset(const asset::Sprite* sprite, bool isCentered) {
        if (!isCentered || !sprite) {
            return slurp::Vec2<float>::Zero;
        }
        return -sprite->dimensions / 2;
    }

    RenderInfo::RenderInfo()
        : renderingEnabled(false),
          sprite(nullptr),
          animation({}),
          zOrder(0),
          renderOffset({}) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder
    )
        : renderingEnabled(true),
          sprite(sprite),
          animation({}),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite, isCentered)) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    )
        : renderingEnabled(true),
          sprite(sprite),
          animation({}),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite, isCentered) + renderOffset) {}
}
