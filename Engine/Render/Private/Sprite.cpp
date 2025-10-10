#include "Sprite.h"

namespace asset {
    // NOTE: represents a rectangle made of 2 triangles
    static constexpr int SpriteMeshVertexCount = 4;
    static constexpr int SpriteMeshElementCount = 6;
    static constexpr uint32_t SpriteElements[SpriteMeshElementCount] = {0, 1, 2, 2, 3, 0};

    void Sprite::draw(const slurp::Vec2<float>& startPoint) const {
        slurp::GlobalRenderApi->drawElementArray(
            mesh.vertexArrayId,
            mesh.elementCount,
            material.textureId,
            material.shaderProgramId,
            startPoint
        );
    }

    Sprite loadSprite(const std::string& spriteFileName) {
        Bitmap bitmap = loadBitmapFile(spriteFileName);

        // TODO: specify scale factor on entity that also applies to collision shapes
        float scale = 1.f;
        slurp::Vec2<float> dimensions = bitmap.dimensions * scale;
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

        render::object_id vertexArrayId = slurp::GlobalRenderApi->genElementArrayBuffer(
            triangleVertices,
            SpriteMeshVertexCount,
            SpriteElements,
            SpriteMeshElementCount
        );

        render::object_id textureId = slurp::GlobalRenderApi->createTexture(bitmap);

        // TODO: allow specification of shader
        render::object_id shaderProgramId = slurp::GlobalRenderApi->loadShaderProgram(
            "sprite.glsl",
            "sprite.glsl"
        );

        return Sprite{
            dimensions,
            Mesh{vertexArrayId, SpriteMeshElementCount},
            Material{textureId, shaderProgramId},
        };
    }
}
