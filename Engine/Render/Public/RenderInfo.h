#pragma once
#include "Sprite.h"

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        const asset::Sprite* sprite;
        RenderShape renderShape;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(const Sprite& sprite, bool isCentered);

        RenderInfo(const RenderShape& renderShape, bool isCentered);

        void draw(const slurp::Vec2<float>& position) const;
    };
}
