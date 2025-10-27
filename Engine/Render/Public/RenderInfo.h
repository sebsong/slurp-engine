#pragma once
#include "Render.h"

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        const asset::Sprite* sprite;
        RenderShape renderShape; // TODO: get rid of this?
        int zOrder;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(const asset::Sprite* sprite, bool isCentered, int zOrder);

        RenderInfo(const asset::Sprite* sprite, bool isCentered, int zOrder, const slurp::Vec2<float>& renderOffset);

        RenderInfo(const RenderShape& renderShape, bool isCentered, int zOrder);

        void draw(const slurp::Vec2<float>& position) const;
    };
}
