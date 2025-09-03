#include "RenderApi.h"

#include "Asset.h"

namespace render {
    object_id RenderApi::loadShaderProgram(
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) const {
        std::string vertexShaderSource = asset::loadVertexShaderSource(vertexShaderFileName);
        std::string fragmentShaderSource = asset::loadFragmentShaderSource(fragmentShaderFileName);

        return createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
    }
}
