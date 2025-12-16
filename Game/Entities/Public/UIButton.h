#pragma once
#include "Entity.h"

namespace ui {
    class UIButton final : public entity::Entity {
    public:
        UIButton(
            asset::Sprite* buttonIconSprite,
            asset::Sprite* buttonSprite,
            asset::Sprite* buttonHoverSprite,
            asset::Sprite* buttonPressSprite,
            const geometry::Shape& buttonShape,
            const slurp::Vec2<float>& position,
            std::optional<slurp::KeyboardCode> keyCode,
            std::function<void(UIButton* button)>&& onPressFn,
        std::function<void(UIButton* button)>&& releaseActionFn,
            std::function<void(UIButton* button)>&& onReleaseFn,
            float pressOffset = -1
        );

        void enableButton();

        void disableButton();

    private:
        bool _isPressed;
        bool _wasPressedByMouse;
        bool _buttonDisabled;
        std::function<void(UIButton* button)> _onPressFn;
        std::function<void(UIButton* button)> _releaseActionFn;
        std::function<void(UIButton* button)> _onReleaseFn;
        std::optional<slurp::KeyboardCode> _keyCode;
        asset::Sprite* _buttonSprite;
        asset::Sprite* _buttonHoverSprite;
        asset::Sprite* _buttonPressedSprite;
        float _pressOffset;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void hover();

        void press();

        void release();
    };
}
