#pragma once

#include "Sprite.h"

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        uint8_t numSprites;
        asset::SpriteInstance* sprites;

        RenderInfo();

        RenderInfo(const asset::SpriteInstance& sprite);

        RenderInfo(uint8_t numSprites, const asset::SpriteInstance* sprites);

        template<size_t N>
        RenderInfo(const asset::SpriteInstance (&sprites)[N]): RenderInfo(N, sprites) {}

        RenderInfo(const RenderInfo& other);

        RenderInfo(const RenderInfo&& other);

        RenderInfo& operator=(const RenderInfo& other);

        RenderInfo& operator=(const RenderInfo&& other);

        ~RenderInfo();
    };
}
