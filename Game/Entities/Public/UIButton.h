#pragma once
#include "Entity.h"

namespace ui_button {
    class UIButton final : public entity::Entity {
    public:
        UIButton(
            asset::Sprite* buttonSprite,
            asset::Sprite* buttonHoverSprite,
            asset::Sprite* buttonPressedSprite,
            slurp::Vec2<float>&& position,
            slurp::KeyboardCode keyCode,
            std::function<void()>&& onClickFn
        );

    private:
        bool _isPressed;
        std::function<void()> _onClickFn;
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
