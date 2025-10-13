#include "RenderApi.h"

#include "Asset.h"

namespace render {
    object_id RenderApi::loadShaderProgram(
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) const {
        std::string vertexShaderSource = slurp::GlobalAssetLoader->loadVertexShaderSource(vertexShaderFileName)->source;
        std::string fragmentShaderSource = slurp::GlobalAssetLoader->loadFragmentShaderSource(fragmentShaderFileName)->source;

        return createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
    }
}
