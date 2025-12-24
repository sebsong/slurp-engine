#include "UIButton.h"

namespace ui {
    static const slurp::Vec2<float> buttonRenderOffset = {0, -0.75f};

    UIButton::UIButton(
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
        std::function<void(UIButton* button)>&& onHoverFn,
        float pressOffset,
        int zOrder
    ) : Entity(
            "UI Button",
            render::RenderInfo(
                (render::SpriteInstance[2]){
                    render::SpriteInstance(
                        buttonSprite,
                        zOrder,
                        buttonRenderOffset
                    ),
                    render::SpriteInstance(
                        buttonIconSprite,
                        zOrder - 1,
                        buttonRenderOffset
                    )
                }
            ),
            physics::PhysicsInfo(position),
            collision::CollisionInfo(
                true,
                true,
                buttonShape,
                true
            )
        ),
        _isPressed(false),
        _wasPressedByMouse(false),
        _isHovered(false),
        _buttonDisabled(false),
        _onPressFn(std::move(onPressFn)),
        _releaseActionFn(std::move(releaseActionFn)),
        _onReleaseFn(std::move(onReleaseFn)),
        _onHoverFn(std::move(onHoverFn)),
        _keyCode(keyCode),
        _buttonSprite(buttonSprite),
        _buttonHoverSprite(buttonHoverSprite),
        _buttonPressedSprite(buttonPressSprite),
        _pressOffset(pressOffset) {}

    void UIButton::enableButton() {
        setAlpha(1.f);
        _buttonDisabled = false;
    }

    void UIButton::disableButton() {
        release();
        setAlpha(0.5f);
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

        bool pressedByKey = _keyCode.has_value() ? keyboardState.isDown(_keyCode.value()) : false;
        bool mouseOnButton = mouseHitTest(mouseState.position);
        bool pressedByMouse = mouseOnButton && mouseState.isDown(slurp::MouseCode::LeftClick);

        if (pressedByKey || pressedByMouse) {
            press();
            _wasPressedByMouse = pressedByMouse;
            return;
        }

        if (
            _isPressed &&
            (!_wasPressedByMouse || (_wasPressedByMouse && mouseOnButton))
        ) {
            _releaseActionFn(this);
        } else if (_isPressed) {
            release();
        }

        if (mouseOnButton) {
            hover();
        } else {
            release();
        }
    }

    void UIButton::hover() {
        setTexture(_buttonHoverSprite);
        if (_isHovered) {
            return;
        }
        _isHovered = true;
        _onHoverFn(this);
    }

    void UIButton::press() {
        setTexture(_buttonPressedSprite);
        if (_isPressed) {
            return;
        }
        _isPressed = true;
        physicsInfo.position.y += _pressOffset;
        _onPressFn(this);
    }

    void UIButton::release() {
        setTexture(_buttonSprite);
        _isHovered = false;
        if (!_isPressed) {
            return;
        }
        _wasPressedByMouse = false;
        _isPressed = false;
        physicsInfo.position.y -= _pressOffset;
        _onReleaseFn(this);
    }
}
