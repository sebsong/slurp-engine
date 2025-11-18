#include "UIButton.h"

namespace ui {
    static const geometry::Shape buttonShape = {geometry::Rect, {25, 16}};
    static const slurp::Vec2<float> buttonRenderOffset = {0, -0.75f};

    UIButton::UIButton(
        asset::Sprite* buttonIconSprite,
        asset::Sprite* buttonSprite,
        asset::Sprite* buttonHoverSprite,
        asset::Sprite* buttonPressSprite,
        slurp::Vec2<float>&& position,
        slurp::KeyboardCode keyCode,
        std::function<void()>&& onPressFn,
        std::function<void()>&& onReleaseFn
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
        _onPressFn(std::move(onPressFn)),
        _onReleaseFn(std::move(onReleaseFn)),
        _keyCode(keyCode),
        _buttonIconSprite(buttonIconSprite),
        _buttonSprite(buttonSprite),
        _buttonHoverSprite(buttonHoverSprite),
        _buttonPressedSprite(buttonPressSprite) {
    }

    void UIButton::enableButton() {
        renderInfo.sprites->material.alpha = 1.f;
        _buttonDisabled = false;
    }

    void UIButton::disableButton() {
        release();
        renderInfo.sprites->material.alpha = 0.5f;
        _buttonDisabled = true;
    }

    void UIButton::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (_buttonDisabled) {
            return;
        }

        if (keyboardState.isDown(_keyCode)) {
            press();
            return;
        }

        if (mouseHitTest(mouseState.position)) {
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
        release();
        setTexture(_buttonHoverSprite);
    }

    void UIButton::press() {
        setTexture(_buttonPressedSprite);
        if (_isPressed) {
            return;
        }
        _isPressed = true;
        _onPressFn();
    }

    void UIButton::release() {
        setTexture(_buttonSprite);
        if (!_isPressed) {
            return;
        }
        _isPressed = false;
        _onReleaseFn();
    }
}
