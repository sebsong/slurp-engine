#pragma once
#include <cstdint>
#include <string>

namespace o_gl {
    class OpenGLShader {
    public:
        OpenGLShader(const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName);

        void use() const;

    private:
        uint32_t _programId;
    };
}
