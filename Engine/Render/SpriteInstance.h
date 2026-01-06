#pragma once
#include "Asset.h"
#include "Material.h"
#include "SpriteAnimation.h"

#define DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME "sprite.glsl"
#define DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME "sprite.glsl"

namespace render {
    // TODO: move to render namespace?
    struct SpriteInstance {
        bool renderingEnabled;
        slurp::Vec2<int> dimensions;
        asset::Mesh mesh;
        asset::Material material;
        asset::SpriteAnimation animation;
        bool syncZOrderToY;
        int zOrder;
        slurp::Vec2<float> renderOffset;

        explicit SpriteInstance(
            const asset::Sprite* sprite,
            const slurp::Vec2<float>& renderOffset = slurp::Vec2<float>::Zero,
            bool isCentered = true
        );

        SpriteInstance(
            const asset::Sprite* sprite,
            int zOrder,
            const slurp::Vec2<float>& renderOffset = slurp::Vec2<float>::Zero,
            bool isCentered = true
        );

        SpriteInstance(
            bool renderingEnabled,
            const asset::Sprite* sprite,
            bool syncZOrderToY,
            int zOrder,
            const slurp::Vec2<float>& renderOffset
        );

        void bindShaderUniform(const char* uniformName, float value) const;

        void bindShaderUniform(const char* uniformName, bool value) const;

        void bindShaderUniform(const char* uniformName, slurp::Vec4<float> value) const;
    };

    void loadSpriteData(
        asset::Sprite* sprite,
        const asset::Bitmap* bitmap,
        object_id shaderProgramId
    );
}
