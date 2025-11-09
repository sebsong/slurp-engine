#include "UIButton.h"

namespace ui {
    static const geometry::Shape buttonShape = {geometry::Rect, {25, 16}};
    static const slurp::Vec2<float> buttonRenderOffset = {0, -0.75f};

    UIButton::UIButton(
        asset::Sprite* buttonSprite,
        asset::Sprite* buttonHoverSprite,
        asset::Sprite* buttonPressedSprite,
        slurp::Vec2<float>&& position,
        slurp::KeyboardCode keyCode,
        std::function<void()>&& onClickFn
    ) : Entity(
            "UI Button",
            render::RenderInfo(
                buttonSprite,
                true,
                game::UI_Z,
                buttonRenderOffset
            ),
            physics::PhysicsInfo(std::move(position)),
            collision::CollisionInfo(
                true,
                true,
                buttonShape,
                true
            )
        ),
        _isPressed(false),
        _onClickFn(std::move(onClickFn)),
        _keyCode(keyCode),
        _buttonSprite(buttonSprite),
        _buttonHoverSprite(buttonHoverSprite),
        _buttonPressedSprite(buttonPressedSprite) {}

    void UIButton::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (keyboardState.isDown(_keyCode)) {
            press();
            return;
        }

        if (hitTest(mouseState.position)) {
            if (mouseState.isDown(slurp::MouseCode::LeftClick)) {
                press();
            } else {
                hover();
            }
            return;
        }

        release();
    }

    void UIButton::hover() {
        _isPressed = false;
        renderInfo.sprite = _buttonHoverSprite;
    }

    void UIButton::press() {
        if (_isPressed) {
            return;
        }
        _isPressed = true;
        _onClickFn();
        renderInfo.sprite = _buttonPressedSprite;
    }

    void UIButton::release() {
        _isPressed = false;
        renderInfo.sprite = _buttonSprite;
    }
}
