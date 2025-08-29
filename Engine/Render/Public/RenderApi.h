#pragma once

#include <cstdint>
#include <string>

#include "Bitmap.h"
#include "DynamicDeclaration.h"
#include "Vector.h"

namespace render {
    typedef uint32_t texture_id;
    typedef uint32_t shader_program_id;
    typedef uint32_t vertex_array_id;

    static constexpr int INVALID_ID = -1;
    static constexpr uint32_t POSITION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t TEXTURE_VERTEX_ATTRIBUTE_IDX = 1;
    static constexpr uint32_t POSITION_ATTRIBUTE_SIZE = sizeof(slurp::Vec3<float>);
    // static constexpr uint32_t TEXTURE_ATTRIBUTE_SIZE = sizeof(slurp::Vec2<float>);
    static constexpr uint32_t TEXTURE_ATTRIBUTE_SIZE = 0;
    static constexpr uint32_t VERTEX_SIZE = POSITION_ATTRIBUTE_SIZE + TEXTURE_ATTRIBUTE_SIZE;
    static constexpr const char* TIME_UNIFORM_NAME = "time";
    static constexpr const char* TEXTURE_UNIFORM_NAME = "texture";

#define RENDER_SET_BACKGROUND_COLOR(fnName) void fnName(float red, float green, float blue)
#define RENDER_CREATE_TEXTURE(fnName) render::texture_id fnName(asset::Bitmap bitmap)
#define RENDER_CREATE_SHADER_PROGRAM(fnName) render::shader_program_id fnName(const char * vertexShaderSource, const char * fragmentShaderSource)
#define RENDER_GEN_ARRAY_BUFFER(fnName) render::vertex_array_id fnName(const slurp::Vec3<float> vertexArray[], int vertexCount)
#define RENDER_DRAW_ARRAY(fnName) void fnName(render::vertex_array_id vertexArrayId, int vertexCount, render::texture_id textureId, render::shader_program_id shaderProgramId)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_SET_BACKGROUND_COLOR, setBackgroundColor)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_CREATE_TEXTURE, createTexture, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_CREATE_SHADER_PROGRAM, createShaderProgram, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_GEN_ARRAY_BUFFER, genArrayBuffer, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_DRAW_ARRAY, drawArray)

    struct RenderApi {
        dyn_setBackgroundColor* setBackgroundColor = stub_setBackgroundColor;
        dyn_createTexture* createTexture = stub_createTexture;
        dyn_createShaderProgram* createShaderProgram = stub_createShaderProgram;
        dyn_genArrayBuffer* genArrayBuffer = stub_genArrayBuffer;
        dyn_drawArray* drawArray = stub_drawArray;

        [[nodiscard]] shader_program_id loadShaderProgram(
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        ) const;
    };
}
