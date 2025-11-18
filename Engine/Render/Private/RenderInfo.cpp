#include "RenderInfo.h"

namespace render {
    static slurp::Vec2<float> getRenderOffset(const asset::Sprite* sprite, bool isCentered) {
        if (!isCentered || !sprite) {
            return slurp::Vec2<float>::Zero;
        }
        return -sprite->dimensions / 2;
    }

    static void copySprites(uint8_t numSprites, const asset::Sprite* src, asset::Sprite* dest) {
        if (src) {
            for (uint8_t i = 0; i < numSprites; i++) {
                new(&dest[i]) asset::Sprite(src[i]);
            }
        }
    }

    RenderInfo::RenderInfo()
        : RenderInfo(
            false,
            0,
            nullptr,
            false,
            0,
            {}
        ) {}

    RenderInfo::RenderInfo(const RenderInfo& other)
        : RenderInfo(
            other.renderingEnabled,
            other.numSprites,
            other.sprites,
            other.syncZOrderToY,
            other.zOrder,
            other.renderOffset
        ) {}

    RenderInfo::RenderInfo(const RenderInfo&& other)
        : RenderInfo(
            std::move(other.renderingEnabled),
            std::move(other.numSprites),
            std::move(other.sprites),
            std::move(other.syncZOrderToY),
            std::move(other.zOrder),
            std::move(other.renderOffset)
        ) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder
    ): RenderInfo(
        true,
        1,
        sprite,
        false,
        zOrder,
        getRenderOffset(sprite, isCentered)
    ) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        const slurp::Vec2<float>& renderOffset
    ): RenderInfo(
        true,
        1,
        sprite,
        true,
        0,
        getRenderOffset(sprite, isCentered) + renderOffset
    ) {}

    RenderInfo::RenderInfo(
        asset::Sprite* sprite,
        bool isCentered,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    ): RenderInfo(
        true,
        1,
        sprite,
        false,
        zOrder,
        getRenderOffset(sprite, isCentered) + renderOffset
    ) {}

    RenderInfo::RenderInfo(
        bool renderingEnabled,
        uint8_t numSprites,
        asset::Sprite* sprites,
        bool syncZOrderToY,
        int zOrder,
        slurp::Vec2<float> renderOffset
    ): renderingEnabled(renderingEnabled),
       numSprites(numSprites),
       sprites(memory::Permanent->allocate<asset::Sprite>(numSprites)),
       syncZOrderToY(syncZOrderToY),
       zOrder(zOrder),
       renderOffset(renderOffset) {
        copySprites(numSprites, sprites, this->sprites);
    }

    RenderInfo& RenderInfo::operator=(const RenderInfo& other) {
        if (this != &other) {
            renderingEnabled = other.renderingEnabled;
            numSprites = other.numSprites;
            copySprites(numSprites, other.sprites, sprites);
            syncZOrderToY = other.syncZOrderToY;
            zOrder = other.zOrder;
            renderOffset = other.renderOffset;
        }
        return *this;
    }

    RenderInfo& RenderInfo::operator=(const RenderInfo&& other) {
        if (this != &other) {
            renderingEnabled = std::move(other.renderingEnabled);
            numSprites = std::move(other.numSprites);
            copySprites(numSprites, std::move(other.sprites), sprites);
            syncZOrderToY = std::move(other.syncZOrderToY);
            zOrder = std::move(other.zOrder);
            renderOffset = std::move(other.renderOffset);
        }
        return *this;
    }

    RenderInfo::~RenderInfo() {
        // NOTE: don't need to free here, should be managed at a higher level
    }
}
