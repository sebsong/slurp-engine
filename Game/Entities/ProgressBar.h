#pragma once
#include "Entity.h"

namespace ui {
    class ProgressBar final : public entity::Entity {
    public:
        float progress;

        ProgressBar() = default;

        ProgressBar(
            slurp::Vec2<float> position,
            float initialProgress,
            bool isVertical,
            asset::Sprite* barSprite,
            asset::Sprite* fillSprite,
            int zOrder
        );

    private:
        Entity _bar;
        Entity _fill;

        void update(float dt) override;
    };
}
