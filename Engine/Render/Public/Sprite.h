#pragma once

#include "Material.h"
#include "Mesh.h"

namespace render {
    struct Sprite {
        asset::Bitmap bitmap; // TODO: this could just be dimensions, or maybe dimensions goes on the Mesh
        Mesh mesh;
        Material material;

        void draw(const slurp::Vec2<float>& startPoint) const;
    };

    Sprite loadSprite(const std::string& spriteFileName);
}
