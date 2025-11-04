#pragma once
#include "Asset.h"

#define DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME "sprite.glsl"
#define DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME "sprite.glsl"

namespace asset {
    struct Sprite : Asset {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;

        void draw(const slurp::Vec2<float>& startPoint, int zOrder) const;

        void bindShaderUniform(const char* uniformName, float value) const;

        void bindShaderUniform(const char* uniformName, bool value) const;
    };

    void loadSpriteData(
        Sprite* sprite,
        const Bitmap* bitmap,
        const std::string& vertexShaderSource,
        const std::string& fragmentShaderSource
    );
}
