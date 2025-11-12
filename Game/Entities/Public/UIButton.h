#pragma once
#include "Entity.h"

namespace ui {
    class UIButton final : public entity::Entity {
    public:
        UIButton(
            asset::Sprite* buttonSprite,
            asset::Sprite* buttonHoverSprite,
            asset::Sprite* buttonPressedSprite,
            slurp::Vec2<float>&& position,
            slurp::KeyboardCode keyCode,
            std::function<void()>&& onPressFn,
            std::function<void()>&& onReleaseFn
        );

    private:
        bool _isPressed;
        std::function<void()> _onPressFn;
        std::function<void()> _onReleaseFn;
        slurp::KeyboardCode _keyCode;
        asset::Sprite* _buttonSprite;
        asset::Sprite* _buttonHoverSprite;
        asset::Sprite* _buttonPressedSprite;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void hover();

        void press();

        void release();
    };
}
