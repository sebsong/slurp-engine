#include "Render.h"

#include "RenderInfo.h"
#include "Sprite.h"

namespace asset {
    struct SpriteAnimation;
    struct Sprite;
}

namespace render {
    void draw(RenderInfo& renderInfo, const slurp::Vec2<float>& position, float dt) {
        if (!renderInfo.renderingEnabled) {
            return;
        }

        slurp::Vec2<float> startPoint = position + renderInfo.renderOffset;
        asset::Sprite* sprite = renderInfo.sprite;
        asset::SpriteAnimation& animation = renderInfo.animation;
        if (animation.isPlaying) {
            animation.update(dt);
        }

        if (sprite && !sprite->dimensions.isZero()) {
            asset::Mesh& mesh = sprite->mesh;
            asset::Material& material = sprite->material;
            object_id textureId = animation.isPlaying
                                      ? animation.textureIds[animation.currentFrameIndex]
                                      : material.textureId;
            slurp::Globals->RenderApi->drawElementArray(
                mesh.vertexArrayId,
                mesh.elementCount,
                textureId,
                material.shaderProgramId,
                startPoint,
                renderInfo.zOrder
            );
        }
    }
}
