#pragma once
#include <cstdint>
#include <string>

namespace open_gl {
    class OpenGLShader {
    public:
        OpenGLShader(const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName);

        void use() const;

    private:
        bool _isValid;
        uint32_t _programId;
    };
}
