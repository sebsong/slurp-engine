#pragma once
#include "Entity.h"
#include "ZOrder.h"

namespace ui {
    class Button final : public entity::Entity {
    public:
        Button() = default;

        Button(
            asset::Sprite* buttonIconSprite,
            asset::Sprite* buttonSprite,
            asset::Sprite* buttonHoverSprite,
            asset::Sprite* buttonPressSprite,
            const geometry::Shape& buttonShape,
            const slurp::Vec2<float>& position,
            std::optional<slurp::KeyboardCode> keyCode,
            std::function<void(Button* button)>&& onPressFn,
            std::function<void(Button* button)>&& releaseActionFn,
            std::function<void(Button* button)>&& onReleaseFn,
            std::function<void(Button* button)>&& onHoverFn,
            float pressOffset = -1,
            int zOrder = game::ZOrder::UI_Z
        );

        void enableButton();

        void disableButton();

    private:
        bool _isPressed;
        bool _wasPressedByMouse;
        bool _isHovered;
        bool _buttonDisabled;
        std::function<void(Button* button)> _onPressFn;
        std::function<void(Button* button)> _releaseActionFn;
        std::function<void(Button* button)> _onReleaseFn;
        std::function<void(Button* button)> _onHoverFn;
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
