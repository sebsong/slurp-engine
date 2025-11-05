#pragma once

#define Z_ORDER_MAX 1000
#include "Sprite.h"
#include "SpriteAnimation.h"

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        asset::Sprite* sprite;
        asset::SpriteAnimation animation;
        int zOrder;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(asset::Sprite* sprite, bool isCentered, int zOrder);

        RenderInfo(asset::Sprite* sprite, bool isCentered, int zOrder, const slurp::Vec2<float>& renderOffset);
    };
}
