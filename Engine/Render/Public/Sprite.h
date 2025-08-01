#pragma once
#include "Asset.h"

namespace render {
    struct Sprite {
        asset::Bitmap bitmap;

        void draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& startPoint) const;
    };

    Sprite loadSprite(const std::string& spriteFileName);
}
