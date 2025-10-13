#pragma once

#include "Mesh.h"
#include "Material.h"

namespace asset {
    // TODO: convert into async loaded asset::Asset
    struct Sprite {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;

        void draw(const slurp::Vec2<float>& startPoint) const;
    };

    // TODO: move to asset loader?
    Sprite loadSprite(const std::string& spriteFileName);
}
