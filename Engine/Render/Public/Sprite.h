#pragma once
#include "Asset.h"

#define DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME "sprite.glsl"
#define DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME "sprite.glsl"

namespace asset {
    void loadSpriteData(
        Sprite* sprite,
        const Bitmap* bitmap,
        const std::string& vertexShaderSource,
        const std::string& fragmentShaderSource
    );
}
