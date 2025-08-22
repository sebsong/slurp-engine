#include "OpenGLShader.h"

#include "Asset.h"
#include "Logging.h"
#include "glad/glad.h"

namespace o_gl {
    OpenGLShader::OpenGLShader(const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName) {
        std::string vertexShaderSource = asset::loadVertexShaderSource(vertexShaderFileName);
        std::string fragmentShaderSource = asset::loadVertexShaderSource(fragmentShaderFileName);
        logging::info("VERTEX: " + vertexShaderSource);
        logging::info("FRAGMENT: " + fragmentShaderSource);
    }

    void OpenGLShader::use() const {
        glUseProgram(_programId);
    }
}
