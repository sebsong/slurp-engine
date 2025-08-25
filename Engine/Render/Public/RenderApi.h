#pragma once

#include <cstdint>
#include <string>

#include "DynamicDeclaration.h"

namespace render {
    typedef uint32_t shader_program_id;

    static constexpr uint32_t INVALID_ID = 0;
    static constexpr uint32_t UNUSED_ID = 0;
    static constexpr uint32_t LOCATION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t COLOR_VERTEX_ATTRIBUTE_IDX = 1;

#define CREATE_SHADER_PROGRAM(fnName) render::shader_program_id fnName(const char * vertexShaderSource, const char * fragmentShaderSource)

    SLURP_DECLARE_DYNAMIC_RETURN(CREATE_SHADER_PROGRAM, createShaderProgram, render::INVALID_ID)

    struct RenderApi {
        dyn_createShaderProgram* createShaderProgram = stub_createShaderProgram;

        shader_program_id loadShaderProgram(
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        ) const;
    };
}
