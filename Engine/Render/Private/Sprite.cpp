#include "Sprite.h"

namespace render {
    // NOTE: represents a rectangle made of 2 triangles
    static constexpr int SpriteMeshVertexCount = 4;
    static constexpr int SpriteMeshElementCount = 6;

    void Sprite::draw(const GraphicsBuffer& buffer, const slurp::Vec2<float>& startPoint) const {
        // const slurp::Vec2<int> roundedStartPoint = static_cast<slurp::Vec2<int>>(startPoint);
        // const slurp::Vec2<float> endPoint = startPoint + bitmap.dimensions;
        // const slurp::Vec2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        // const slurp::Vec2<int> clampedEndPoint = _getClamped(buffer, endPoint);
        //
        // for (int y = clampedStartPoint.y; y < clampedEndPoint.y; y++) {
        //     for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) {
        //         Pixel pixel = bitmap.map[
        //             (y - roundedStartPoint.y) * bitmap.dimensions.width + (x - roundedStartPoint.x)
        //         ];
        //         _drawAtPoint(
        //             buffer,
        //             {x, y},
        //             pixel
        //         );
        //     }
        // }
        slurp::GlobalRenderApi->drawElementArray(
            mesh.vertexArrayId,
            mesh.elementCount,
            material.textureId,
            material.shaderProgramId
        );
    }

    Sprite loadSprite(const std::string& spriteFileName) {
        asset::Bitmap bitmap = asset::loadBitmapFile(spriteFileName);

        // TODO: specify scale factor
        float scale = 5.f;
        slurp::Vec2<float> dimensions = bitmap.dimensions * scale;
        Vertex triangleVertices[SpriteMeshVertexCount] = {
            Vertex{
                {dimensions.width, dimensions.height, 0},
                {1, 1}
            },
            Vertex{
                {0, dimensions.height, 0},
                {0, 1}
            },
            Vertex{
                {0, 0, 0},
                {0, 0}
            },
            Vertex{
                {dimensions.width, 0, 0},
                {1, 0}
            },
        };

        const uint32_t triangleElements[SpriteMeshElementCount] = {0, 1, 2, 2, 3, 0};
        object_id vertexArrayId = slurp::GlobalRenderApi->genElementArrayBuffer(
            triangleVertices,
            SpriteMeshVertexCount,
            triangleElements,
            SpriteMeshElementCount
        );
        object_id textureId = slurp::GlobalRenderApi->createTexture(bitmap);

        // TODO: allow specification of shader
        object_id shaderProgramId = slurp::GlobalRenderApi->loadShaderProgram(
            "default.glsl",
            "default.glsl"
        );

        return Sprite{
            bitmap,
            Mesh{vertexArrayId, SpriteMeshElementCount},
            Material{textureId, shaderProgramId},
        };
    }
}
