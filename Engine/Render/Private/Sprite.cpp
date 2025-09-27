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

        // TODO: specify these in world coords, have the graphics api layer convert to homogenous clip space coords
        const Vertex triangleVertices[SpriteMeshVertexCount] = {
            Vertex{{0.5f, 0.5f, 0.f}, {1, 1}},
            Vertex{{-0.5f, 0.5f, 0.f}, {0, 1}},
            Vertex{{-0.5f, -0.5f, 0.f}, {0, 0}},
            Vertex{{0.5f, -0.5f, 0.f}, {1, 0}},
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
            "tutorial.glsl",
            "tutorial.glsl"
        );

        return Sprite{
            bitmap,
            Mesh{vertexArrayId, SpriteMeshElementCount},
            Material{textureId, shaderProgramId},
        };
    }
}
