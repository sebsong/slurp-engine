#pragma once
#include "Render.h"
#include "Sprite.h"

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        Sprite sprite; // TODO: should this be a reference for faster sprite swapping?
        RenderShape renderShape;
        slurp::Vector2<int> renderOffset;

        RenderInfo();

        RenderInfo(std::string&& spriteFileName, bool isCentered);

        RenderInfo(const Sprite& sprite, bool isCentered);

        RenderInfo(const RenderShape& renderShape, bool isCentered);

        void draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) const;
    };

}
