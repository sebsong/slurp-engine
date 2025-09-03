#include "RenderInfo.h"

#include "Asset.h"

namespace render {
    static slurp::Vec2<float> getRenderOffset(const slurp::Vec2<float>& dimensions, bool isCentered) {
        if (!isCentered) {
            return slurp::Vec2<float>::Zero;
        }
        return -dimensions / 2;
    }

    RenderInfo::RenderInfo()
        : renderingEnabled(false),
          openGLInfo({}),
          sprite({}),
          renderShape({}),
          renderOffset({}) {}

    RenderInfo::RenderInfo(const Sprite& sprite, bool isCentered)
        : renderingEnabled(true),
          openGLInfo({}),
          sprite(sprite),
          renderShape({}),
          renderOffset(getRenderOffset(sprite.bitmap.dimensions, isCentered)) {}

    RenderInfo::RenderInfo(
        const RenderShape& renderShape,
        bool isCentered
    ): renderingEnabled(true),
       openGLInfo({}),
       sprite({}),
       renderShape(renderShape),
       renderOffset(getRenderOffset(renderShape.shape.dimensions, isCentered)) {}

    RenderInfo::RenderInfo(const open_gl::OpenGLRenderInfo& openGLInfo, bool isCentered)
        : renderingEnabled(true),
          openGLInfo(openGLInfo),
          renderShape({}),
          renderOffset(getRenderOffset(sprite.bitmap.dimensions, isCentered)) {}

    void RenderInfo::draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& position) const {
        if (!renderingEnabled) { return; }

        slurp::Vec2<float> startPoint = position + renderOffset;
        if (sprite.bitmap.map) { sprite.draw(buffer, startPoint); } else { renderShape.draw(buffer, startPoint); }
    }

    void RenderInfo::draw(const slurp::Vec2<float>& position, const RenderApi* renderApi) const {
        // TODO: handle position
        // renderApi->drawArray(openGLInfo.vertexArrayObjectId, openGLInfo.elementCount, openGLInfo.textureId, openGLInfo.shaderProgramId);
        renderApi->drawElementArray(
            openGLInfo.vertexArrayObjectId,
            openGLInfo.elementCount,
            openGLInfo.textureId,
            openGLInfo.shaderProgramId
        );
    }
}
