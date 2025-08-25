#include "OpenGLShader.h"

#include "Asset.h"
#include "glad/glad.h"

namespace open_gl {
    OpenGLShader::OpenGLShader(const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName) {
        const char* vertexShaderSource = asset::loadVertexShaderSource(vertexShaderFileName).c_str();
        const char* fragmentShaderSource = asset::loadVertexShaderSource(fragmentShaderFileName).c_str();

        this->_programId = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }

    void OpenGLShader::use() const {
        ASSERT(this->_isValid);
        glUseProgram(_programId);
    }
}
