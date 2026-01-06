#pragma once

#include "Debug.h"
#include "DynamicDeclaration.h"
#include "Vector.h"

#include <cstdint>

namespace asset {
    struct Bitmap;
}

namespace render {
    typedef uint32_t object_id;

    // TODO: does this need to be [[gnu::packed]]?
    struct Vertex {
        slurp::Vec2<float> position;
        slurp::Vec2<float> textureCoord;
    };

    static constexpr int INVALID_OBJECT_ID = -1;
    static constexpr uint32_t POSITION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX = 1;
    static constexpr const char* TIME_UNIFORM_NAME = "time";
    static constexpr const char* POSITION_TRANSFORM_UNIFORM_NAME = "positionTransform";
    static constexpr const char* Z_ORDER_UNIFORM_NAME = "zOrder";
    static constexpr const char* ALPHA_COORD_UNIFORM_NAME = "alpha";
    static constexpr const char* SRC_COLOR_UNIFORM_NAME = "srcColor";
    static constexpr const char* DST_COLOR_UNIFORM_NAME = "dstColor";
    static constexpr const char* COLOR_UNIFORM_NAME = "color";

    void setBackgroundColor(float red, float green, float blue);

    object_id createTexture(const asset::Bitmap* bitmap);

    object_id createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

    void bindShaderUniformFloat(object_id shaderProgramId, const char* uniformName, float value);

    void bindShaderUniformBool(object_id shaderProgramId, const char* uniformName, bool value);

    void bindShaderUniformVec4(object_id shaderProgramId, const char* uniformName, slurp::Vec4<float> value);

    object_id genVertexArrayBuffer(Vertex vertexArray[], int vertexCount);

    object_id genElementArrayBuffer(
        Vertex vertexArray[],
        int vertexCount,
        const uint32_t elementArray[],
        int elementCount
    );

    void drawVertexArray(
        object_id vertexArrayId,
        int vertexCount,
        object_id textureId,
        object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        float alpha,
        int zOrder
    );

    void drawElementArray(
        object_id vertexArrayId,
        int elementCount,
        object_id textureId,
        object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        float alpha,
        int zOrder
    );

    void drawPoint(
        object_id vertexArrayId,
        int vertexCount,
        object_id shaderProgramId,
        float size,
        const slurp::Vec4<float>& color
    );

    void drawLine(
        object_id vertexArrayId,
        int vertexCount,
        object_id shaderProgramId,
        float width,
        const slurp::Vec4<float>& color
    );

    void deleteResources(
        object_id vertexArrayId,
        object_id vertexBufferId,
        object_id elementBufferId,
        object_id shaderProgramId,
        object_id textureId
    );
}
