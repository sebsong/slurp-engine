#pragma once

#include "SpriteInstance.h"

#define Z_ORDER_MAX 1000

namespace asset {
    struct Sprite;
}

namespace render {
    struct RenderInfo {
        uint8_t numSprites;
        SpriteInstance* sprites;

        RenderInfo();

        RenderInfo(const SpriteInstance& sprite);

        RenderInfo(uint8_t numSprites, const SpriteInstance* sprites);

        template<size_t N>
        RenderInfo(const SpriteInstance (&sprites)[N]): RenderInfo(N, sprites) {}

        RenderInfo(uint8_t numSprites);

        RenderInfo(const RenderInfo& other);

        RenderInfo(const RenderInfo&& other);

        RenderInfo& operator=(const RenderInfo& other);

        RenderInfo& operator=(const RenderInfo&& other);

        ~RenderInfo();
    };
}
