#include "Render.h"

#include "RenderInfo.h"
#include "Sprite.h"

namespace asset {
    struct SpriteAnimation;
    struct Sprite;
}

namespace render {
    void draw(RenderInfo& renderInfo, const slurp::Vec2<float>& position, float dt) {
        if (!renderInfo.renderingEnabled || !renderInfo.sprites) {
            return;
        }

        slurp::Vec2<float> startPoint = position + renderInfo.renderOffset;

        for (int i = 0; i < renderInfo.numSprites; i++) {
            asset::Sprite& sprite = renderInfo.sprites[i];
            asset::SpriteAnimation& animation = sprite.animation;
            if (animation.isPlaying) {
                animation.update(dt);
            }

            if (!sprite.dimensions.isZero()) {
                asset::Mesh& mesh = sprite.mesh;
                asset::Material& material = sprite.material;
                object_id textureId = animation.isPlaying
                                          ? animation.textureIds[animation.currentFrameIndex]
                                          : material.textureId;
                slurp::Globals->RenderApi->drawElementArray(
                    mesh.vertexArrayId,
                    mesh.elementCount,
                    textureId,
                    material.shaderProgramId,
                    startPoint,
                    material.alpha,
                    renderInfo.zOrder
                );
            }
        }
    }
}
