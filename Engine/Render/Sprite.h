#pragma once
#include "Asset.h"
#include "SpriteAnimation.h"

#define DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME "sprite.glsl"
#define DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME "sprite.glsl"

namespace asset {
    struct Sprite : Asset {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;
    };

    // TODO: move to render namespace?
    struct SpriteInstance {
        bool renderingEnabled;
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;
        SpriteAnimation animation;
        bool syncZOrderToY;
        int zOrder;
        slurp::Vec2<float> renderOffset;

        explicit SpriteInstance(
            const Sprite* sprite,
            const slurp::Vec2<float>& renderOffset = slurp::Vec2<float>::Zero,
            bool isCentered = true
        );

        SpriteInstance(
            const Sprite* sprite,
            int zOrder,
            const slurp::Vec2<float>& renderOffset = slurp::Vec2<float>::Zero,
            bool isCentered = true
        );

        SpriteInstance(
            bool renderingEnabled,
            const Sprite* sprite,
            bool syncZOrderToY,
            int zOrder,
            const slurp::Vec2<float>& renderOffset
        );

        void bindShaderUniform(const char* uniformName, float value) const;

        void bindShaderUniform(const char* uniformName, bool value) const;
    };

    void loadSpriteData(
        Sprite* sprite,
        const Bitmap* bitmap,
        render::object_id shaderProgramId
    );
}
