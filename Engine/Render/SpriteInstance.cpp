#include "SpriteInstance.h"

#include "Bitmap.h"

namespace render {
    // NOTE: represents a rectangle made of 2 triangles
    static constexpr int SpriteMeshVertexCount = 4;
    static constexpr int SpriteMeshElementCount = 6;
    static constexpr uint32_t SpriteElements[SpriteMeshElementCount] = {0, 1, 2, 2, 3, 0};

    static slurp::Vec2<float> getRenderOffset(const asset::Sprite* sprite, bool isCentered) {
        if (!isCentered || !sprite) {
            return slurp::Vec2<float>::Zero;
        }
        return -sprite->dimensions / 2;
    }

    SpriteInstance::SpriteInstance(
        const asset::Sprite* sprite,
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
        const asset::Sprite* sprite,
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
        const asset::Sprite* sprite,
        bool syncZOrderToY,
        int zOrder,
        const slurp::Vec2<float>& renderOffset
    ): renderingEnabled(renderingEnabled),
       dimensions(sprite ? sprite->dimensions : slurp::Vec2<int>::Zero),
       mesh(sprite ? sprite->mesh : asset::Mesh{}),
       material(sprite ? sprite->material : asset::Material{}),
       animation({}),
       syncZOrderToY(syncZOrderToY),
       zOrder(zOrder),
       renderOffset(renderOffset) {}

    void SpriteInstance::bindShaderUniform(const char* uniformName, float value) const {
        bindShaderUniformFloat(material.shaderProgramId, uniformName, value);
    }

    void SpriteInstance::bindShaderUniform(const char* uniformName, bool value) const {
        bindShaderUniformBool(material.shaderProgramId, uniformName, value);
    }

    void SpriteInstance::bindShaderUniform(const char* uniformName, slurp::Vec4<float> value) const {
        bindShaderUniformVec4(material.shaderProgramId, uniformName, value);
    }

    void loadSpriteData(
        asset::Sprite* sprite,
        const asset::Bitmap* bitmap,
        object_id shaderProgramId
    ) {
        // TODO: specify scale factor on entity that also applies to collision shapes
        float scale = 1.f;
        slurp::Vec2<float> dimensions = bitmap->dimensions * scale;
        Vertex triangleVertices[SpriteMeshVertexCount] = {
            Vertex{
                {dimensions.width, dimensions.height},
                {1, 1}
            },
            Vertex{
                {0, dimensions.height},
                {0, 1}
            },
            Vertex{
                {0, 0},
                {0, 0}
            },
            Vertex{
                {dimensions.width, 0},
                {1, 0}
            },
        };

        object_id vertexArrayId = genElementArrayBuffer(
            triangleVertices,
            SpriteMeshVertexCount,
            SpriteElements,
            SpriteMeshElementCount
        );
        object_id textureId = createTexture(bitmap);

        sprite->dimensions = dimensions;
        sprite->mesh = asset::Mesh{vertexArrayId, SpriteMeshElementCount};
        sprite->material = asset::Material{textureId, shaderProgramId, {}, {}, {}, 1.f};
        sprite->isLoaded = true;
    }
}
