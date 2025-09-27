#pragma once

#include "Material.h"
#include "Mesh.h"

namespace render {
    struct Sprite {
        asset::Bitmap bitmap;
        Mesh mesh;
        Material material;

        void draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& startPoint) const;
    };

    Sprite loadSprite(const std::string& spriteFileName);
}
