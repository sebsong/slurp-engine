#include "ProgressBar.h"

#include "AssetLoader.h"

namespace ui {
    // ProgressBar::ProgressBar(): Entity("ProgressBar") {}
    static const char* ProgressUniformName = "progress";
    static const char* IsVerticalUniformName = "isVertical";

    ProgressBar::ProgressBar(
        slurp::Vec2<float> position,
        float initialProgress,
        bool isVertical,
        asset::Sprite* barSprite,
        asset::Sprite* fillSprite,
        int zOrder
    ): Entity(
           "ProgressBar"
       ),
       progress(initialProgress),
       _bar(
           Entity(
               "Bar",
               render::RenderInfo(render::SpriteInstance(barSprite, zOrder)),
               physics::PhysicsInfo(position),
               {}
           )
       ),
       _fill(
           Entity(
               "Fill",
               render::RenderInfo(render::SpriteInstance(fillSprite, zOrder)),
               physics::PhysicsInfo(position),
               {}
           )
       ) {
        render::object_id progressBarShaderId = asset::loadShaderProgram(
            DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME,
            "progress_bar.glsl"
        )->programId;
        _fill.renderInfo.sprites->material.shaderProgramId = progressBarShaderId;
        _fill.renderInfo.sprites->bindShaderUniform(IsVerticalUniformName, isVertical);
    }


    void ProgressBar::update(float dt) {
        Entity::update(dt);
        _fill.renderInfo.sprites->bindShaderUniform(ProgressUniformName, progress);
    }
}
