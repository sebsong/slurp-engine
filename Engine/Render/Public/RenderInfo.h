#pragma once

#include "Sprite.h"
#include "SpriteAnimation.h"

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        uint8_t numSprites;
        asset::Sprite* sprites;
        asset::SpriteAnimation animation; // TODO: maybe need array of these to match sprites or put inside of sprite
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
