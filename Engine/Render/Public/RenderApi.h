#pragma once

#include <cstdint>
#include <string>

#include "DynamicDeclaration.h"
#include "Vector.h"

namespace render {
    typedef uint32_t shader_program_id;
    typedef uint32_t vertexArrayId;

    static constexpr int INVALID_ID = -1;
    static constexpr uint32_t LOCATION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t COLOR_VERTEX_ATTRIBUTE_IDX = 1;
    static constexpr const char* TIME_UNIFORM_NAME = "time";

#define RENDER_SET_BACKGROUND_COLOR(fnName) void fnName(float red, float green, float blue)
#define RENDER_GEN_ARRAY_BUFFER(fnName) render::vertexArrayId fnName(const slurp::Vec3<float> vertexArray[], int vertexCount)
#define RENDER_CREATE_SHADER_PROGRAM(fnName) render::shader_program_id fnName(const char * vertexShaderSource, const char * fragmentShaderSource)
#define RENDER_DRAW_ARRAY(fnName) void fnName(render::vertexArrayId vertexArrayId, int vertexCount, render::shader_program_id shaderProgramId)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_SET_BACKGROUND_COLOR, setBackgroundColor)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_GEN_ARRAY_BUFFER, genArrayBuffer, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_RETURN(RENDER_CREATE_SHADER_PROGRAM, createShaderProgram, render::INVALID_ID)

    SLURP_DECLARE_DYNAMIC_VOID(RENDER_DRAW_ARRAY, drawArray)

    struct RenderApi {
        dyn_setBackgroundColor* setBackgroundColor = stub_setBackgroundColor;
        dyn_genArrayBuffer* genArrayBuffer = stub_genArrayBuffer;
        dyn_createShaderProgram* createShaderProgram = stub_createShaderProgram;
        dyn_drawArray* drawArray = stub_drawArray;

        [[nodiscard]] shader_program_id loadShaderProgram(
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        ) const;
    };
}
