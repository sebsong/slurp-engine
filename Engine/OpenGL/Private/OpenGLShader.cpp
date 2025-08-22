#include "OpenGLShader.h"

#include "Asset.h"
#include "glad/glad.h"

namespace open_gl {
    OpenGLShader::OpenGLShader(const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName) {
        const char* vertexShaderSource = asset::loadVertexShaderSource(vertexShaderFileName).c_str();
        const char* fragmentShaderSource = asset::loadVertexShaderSource(fragmentShaderFileName).c_str();

        // TODO: make this a createShader method in OpenGL.cpp
        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShaderId);
        this->_isValid = validateShader(vertexShaderId);

        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShaderId);
        this->_isValid = validateShader(fragmentShaderId);

        this->_programId = glCreateProgram();
        glAttachShader(this->_programId, vertexShaderId);
        glAttachShader(this->_programId, fragmentShaderId);
        glLinkProgram(this->_programId);
        this->_isValid = validateShader(this->_programId);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
    }

    void OpenGLShader::use() const {
        ASSERT(this->_isValid);
        glUseProgram(_programId);
    }
}
