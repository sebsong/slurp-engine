#pragma once

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        const asset::Sprite* sprite;
        int zOrder;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(const asset::Sprite* sprite, bool isCentered, int zOrder);

        RenderInfo(const asset::Sprite* sprite, bool isCentered, int zOrder, const slurp::Vec2<float>& renderOffset);
    };

    void draw(const RenderInfo& renderInfo, const slurp::Vec2<float>& position);
}
