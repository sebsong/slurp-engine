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
          numSprites(1),
          sprites(nullptr),
          animation({}),
          syncZOrderToY(false),
          zOrder(0),
          renderOffset({}) {}

    RenderInfo::RenderInfo(asset::Sprite* sprite, bool isCentered)
        : renderingEnabled(true),
          numSprites(1),
          sprites(&sprite),
          animation({}),
          syncZOrderToY(true),
          zOrder(0),
          renderOffset(getRenderOffset(sprite, isCentered)) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder
    )
        : renderingEnabled(true),
          numSprites(1),
          sprites(&sprite),
          animation({}),
          syncZOrderToY(false),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite, isCentered)) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        const slurp::Vec2<float>& renderOffset
    )
        : renderingEnabled(true),
          numSprites(1),
          sprites(&sprite),
          animation({}),
          syncZOrderToY(true),
          zOrder(0),
          renderOffset(getRenderOffset(sprite, isCentered) + renderOffset) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    )
        : renderingEnabled(true),
          numSprites(1),
          sprites(&sprite),
          animation({}),
          syncZOrderToY(false),
          zOrder(zOrder),
          renderOffset(getRenderOffset(sprite, isCentered) + renderOffset) {}
}
