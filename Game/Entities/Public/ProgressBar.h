#pragma once
#include "Entity.h"

namespace ui {
    class ProgressBar final : public entity::Entity {
    public:
        float progress;

        ProgressBar(
            slurp::Vec2<float> position,
            float initialProgress,
            asset::Sprite* barSprite,
            asset::Sprite* fillSprite
        );

    private:
        Entity _bar;
        Entity _fill;

        void update(float dt) override;
    };
}
