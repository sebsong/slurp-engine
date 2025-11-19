#include "Sprite.h"

namespace asset {
    // NOTE: represents a rectangle made of 2 triangles
    static constexpr int SpriteMeshVertexCount = 4;
    static constexpr int SpriteMeshElementCount = 6;
    static constexpr uint32_t SpriteElements[SpriteMeshElementCount] = {0, 1, 2, 2, 3, 0};

    static slurp::Vec2<float> getRenderOffset(const Sprite* sprite, bool isCentered) {
        if (!isCentered || !sprite) {
            return slurp::Vec2<float>::Zero;
        }
        return -sprite->dimensions / 2;
    }

    SpriteInstance::SpriteInstance(
        const Sprite* sprite,
        const slurp::Vec2<float>& renderOffset,
        bool isCentered
    ): SpriteInstance(
        true,
        sprite,
        true,
        0,
        (isCentered ? getRenderOffset(sprite, isCentered) : slurp::Vec2<float>::Zero) + renderOffset
    ) {}

    SpriteInstance::SpriteInstance(
        const Sprite* sprite,
        int zOrder,
        const slurp::Vec2<float>& renderOffset,
        bool isCentered
    ): SpriteInstance(
        true,
        sprite,
        false,
        zOrder,
        (isCentered ? getRenderOffset(sprite, isCentered) : slurp::Vec2<float>::Zero) + renderOffset
    ) {}

    SpriteInstance::SpriteInstance(
        bool renderingEnabled,
        const Sprite* sprite,
        bool syncZOrderToY,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    ): renderingEnabled(renderingEnabled),
       dimensions(sprite ? sprite->dimensions : slurp::Vec2<int>::Zero),
       mesh(sprite ? sprite->mesh : Mesh{}),
       material(sprite ? sprite->material : Material{}),
       animation({}),
       syncZOrderToY(syncZOrderToY),
       zOrder(zOrder),
       renderOffset(renderOffset) {}

    void SpriteInstance::bindShaderUniform(const char* uniformName, float value) const {
        slurp::Globals->RenderApi->bindShaderUniformFloat(material.shaderProgramId, uniformName, value);
    }

    void SpriteInstance::bindShaderUniform(const char* uniformName, bool value) const {
        slurp::Globals->RenderApi->bindShaderUniformBool(material.shaderProgramId, uniformName, value);
    }

    void loadSpriteData(
        Sprite* sprite,
        const Bitmap* bitmap,
        render::object_id shaderProgramId
    ) {
        // TODO: specify scale factor on entity that also applies to collision shapes
        float scale = 1.f;
        slurp::Vec2<float> dimensions = bitmap->dimensions * scale;
        render::Vertex triangleVertices[SpriteMeshVertexCount] = {
            render::Vertex{
                {dimensions.width, dimensions.height},
                {1, 1}
            },
            render::Vertex{
                {0, dimensions.height},
                {0, 1}
            },
            render::Vertex{
                {0, 0},
                {0, 0}
            },
            render::Vertex{
                {dimensions.width, 0},
                {1, 0}
            },
        };

        render::object_id vertexArrayId = slurp::Globals->RenderApi->genElementArrayBuffer(
            triangleVertices,
            SpriteMeshVertexCount,
            SpriteElements,
            SpriteMeshElementCount
        );
        render::object_id textureId = slurp::Globals->RenderApi->createTexture(bitmap);

        sprite->dimensions = dimensions;
        sprite->mesh = Mesh{vertexArrayId, SpriteMeshElementCount};
        sprite->material = Material{textureId, shaderProgramId, 1.f};
        sprite->isLoaded = true;
    }
}
