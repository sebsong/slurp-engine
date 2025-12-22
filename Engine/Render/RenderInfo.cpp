#include "RenderInfo.h"

namespace render {
    static void copySprites(uint8_t numSprites, const SpriteInstance* src, SpriteInstance* dest) {
        if (src) {
            for (uint8_t i = 0; i < numSprites; i++) {
                new(&dest[i]) SpriteInstance(src[i]);
            }
        }
    }

    RenderInfo::RenderInfo()
        : RenderInfo(
            0,
            nullptr
        ) {}

    RenderInfo::RenderInfo(const SpriteInstance& sprite)
        : RenderInfo(
            1,
            &sprite
        ) {}

    RenderInfo::RenderInfo(uint8_t numSprites, const SpriteInstance* sprites)
        : numSprites(numSprites),
          sprites(memory::Permanent->allocate<SpriteInstance>(numSprites)) {
        if (sprites) {
            copySprites(numSprites, sprites, this->sprites);
        }
    }

    RenderInfo::RenderInfo(const RenderInfo& other)
        : RenderInfo(
            other.numSprites,
            other.sprites
        ) {}

    RenderInfo::RenderInfo(const RenderInfo&& other)
        : RenderInfo(
            std::move(other.numSprites),
            std::move(other.sprites)
        ) {}

    RenderInfo& RenderInfo::operator=(const RenderInfo& other) {
        if (this != &other) {
            numSprites = other.numSprites;
            copySprites(numSprites, other.sprites, sprites);
        }
        return *this;
    }

    RenderInfo& RenderInfo::operator=(const RenderInfo&& other) {
        if (this != &other) {
            numSprites = std::move(other.numSprites);
            copySprites(numSprites, std::move(other.sprites), sprites);
        }
        return *this;
    }

    RenderInfo::~RenderInfo() {
        // NOTE: don't need to free here, should be managed at a higher level
    }
}
