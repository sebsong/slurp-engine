#include "ProgressBar.h"

namespace ui {
    // ProgressBar::ProgressBar(): Entity("ProgressBar") {}
    static const char* ProgressUniformName = "progress";

    ProgressBar::ProgressBar(
        slurp::Vec2<float> position,
        float initialProgress,
        asset::Sprite* barSprite,
        asset::Sprite* fillSprite
    ): Entity(
           "ProgressBar"
       ),
       progress(initialProgress),
       _bar(
           Entity(
               "Bar",
               render::RenderInfo(barSprite, true, game::UI_Z),
               physics::PhysicsInfo(position),
               {}
           )
       ),
       _fill(
           Entity(
               "Fill",
               render::RenderInfo(fillSprite, true, game::UI_Z),
               physics::PhysicsInfo(position),
               {}
           )
       ) {
        render::object_id progressBarShaderId = asset::loadShaderProgram(
            DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME,
            "progress_bar.glsl"
        )->programId;
        _fill.renderInfo.sprite->material.shaderProgramId = progressBarShaderId;
    }


    void ProgressBar::update(float dt) {
        Entity::update(dt);
        _fill.renderInfo.sprite->bindShaderUniform(ProgressUniformName, progress);
    }
}
