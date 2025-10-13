#pragma once
#include "Render.h"

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        const asset::Sprite* sprite;
        RenderShape renderShape;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(const asset::Sprite& sprite, bool isCentered);

        RenderInfo(const RenderShape& renderShape, bool isCentered);

        void draw(const slurp::Vec2<float>& position) const;
    };
}
