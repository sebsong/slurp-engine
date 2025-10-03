#pragma once

#include <cstdint>
#include <string>

#include "Bitmap.h"
#include "DynamicDeclaration.h"
#include "Vector.h"

namespace render {
    typedef uint32_t object_id;

    struct Vertex {
        slurp::Vec3<float> position;
        slurp::Vec2<float> textureCoord;
    };


    static constexpr int INVALID_ID = -1;
    static constexpr uint32_t POSITION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX = 1;
    static constexpr const char* TIME_UNIFORM_NAME = "time";
    static constexpr const char* POSITION_TRANSFORM_UNIFORM_NAME = "positionTransform";
    static constexpr const char* TEXTURE_COORD_UNIFORM_NAME = "textureCoord";

#define RENDER_SET_BACKGROUND_COLOR(fnName) void fnName(float red, float green, float blue)
#define RENDER_CREATE_TEXTURE(fnName) render::object_id fnName(asset::Bitmap bitmap)
#define RENDER_CREATE_SHADER_PROGRAM(fnName) render::object_id fnName(const char* vertexShaderSource, const char* fragmentShaderSource)
#define RENDER_GEN_ARRAY_BUFFER(fnName) render::object_id fnName(render::Vertex vertexArray[], int vertexCount)
#define RENDER_GEN_ELEMENT_ARRAY_BUFFER(fnName) render::object_id fnName(render::Vertex vertexArray[], int vertexCount, const uint32_t elementArray[], int elementCount)
#define RENDER_DRAW_ARRAY(fnName) void fnName(render::object_id vertexArrayId, int vertexCount, render::object_id textureId, render::object_id shaderProgramId, const slurp::Vec2<float>& positionTransform)
#define RENDER_DRAW_ELEMENT_ARRAY(fnName) void fnName(render::object_id vertexArrayId, int elementCount, render::object_id textureId, render::object_id shaderProgramId, const slurp::Vec2<float>& positionTransform)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_SET_BACKGROUND_COLOR, setBackgroundColor)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_CREATE_TEXTURE, createTexture, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_CREATE_SHADER_PROGRAM, createShaderProgram, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_GEN_ARRAY_BUFFER, genArrayBuffer, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_GEN_ELEMENT_ARRAY_BUFFER, genElementArrayBuffer, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_DRAW_ARRAY, drawArray)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_DRAW_ELEMENT_ARRAY, drawElementArray)

    struct RenderApi {
        dyn_setBackgroundColor* setBackgroundColor = stub_setBackgroundColor;
        dyn_createTexture* createTexture = stub_createTexture;
        dyn_createShaderProgram* createShaderProgram = stub_createShaderProgram;
        dyn_genArrayBuffer* genArrayBuffer = stub_genArrayBuffer;
        dyn_genElementArrayBuffer* genElementArrayBuffer = stub_genElementArrayBuffer;
        dyn_drawArray* drawArray = stub_drawArray;
        dyn_drawElementArray* drawElementArray = stub_drawElementArray;

        [[nodiscard]] object_id loadShaderProgram(
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        ) const;
    };
}
