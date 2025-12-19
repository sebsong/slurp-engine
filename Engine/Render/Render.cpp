#include "Render.h"

#include "Sprite.h"

namespace render {
    void draw(const slurp::Vec2<float>& position, asset::SpriteInstance& sprite, asset::SpriteAnimation& animation) {
        if (sprite.renderingEnabled && !sprite.dimensions.isZero()) {
            asset::Mesh& mesh = sprite.mesh;
            asset::Material& material = sprite.material;
            object_id textureId = animation.isPlaying
                                      ? animation.textureIds[animation.currentFrameIndex]
                                      : material.textureId;
            slurp::Vec2<float> positionTransform = position + sprite.renderOffset;
            slurp::Globals->RenderApi->drawElementArray(
                mesh.vertexArrayId,
                mesh.elementCount,
                textureId,
                material.shaderProgramId,
                positionTransform,
                material.alpha,
                sprite.zOrder
            );
        }
    }
}
