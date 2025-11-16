#pragma once

#include "Sprite.h"

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        uint8_t numSprites;
        asset::Sprite* sprites;
        bool syncZOrderToY;
        int zOrder;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(asset::Sprite* sprite, bool isCentered);

        RenderInfo(asset::Sprite* sprite, bool isCentered, int zOrder);

        RenderInfo(asset::Sprite* sprite, bool isCentered, const slurp::Vec2<float>& renderOffset);

        RenderInfo(asset::Sprite* sprite, bool isCentered, int zOrder, const slurp::Vec2<float>& renderOffset);
    };
}
