#pragma once
#include "Render.h"
#include "Sprite.h"

namespace render {
    struct RenderInfo {
        bool renderingEnabled;
        open_gl::OpenGLRenderInfo openGLInfo;
        Sprite sprite; // TODO: should this be a reference for faster sprite swapping?
        RenderShape renderShape;
        slurp::Vec2<float> renderOffset;

        RenderInfo();

        RenderInfo(const Sprite& sprite, bool isCentered);

        RenderInfo(const RenderShape& renderShape, bool isCentered);

        RenderInfo(const open_gl::OpenGLRenderInfo& openGLInfo, bool isCentered);

        void draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& position) const;

        void draw(const slurp::Vec2<float>& position, const RenderApi* renderApi) const;
    };
}
