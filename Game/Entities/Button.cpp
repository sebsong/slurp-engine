#include "Button.h"

namespace ui {
    static const slurp::Vec2<float> buttonRenderOffset = {0, -0.75f};

    Button::Button(
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

    void Button::enableButton() {
        setAlpha(1.f);
        _buttonDisabled = false;
    }

    void Button::disableButton() {
        release();
        setAlpha(0.5f);
        _buttonDisabled = true;
    }

    void Button::handleMouseAndKeyboardInput(
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

    void Button::hover() {
        setTexture(_buttonHoverSprite);
        if (_isHovered) {
            return;
        }
        _isHovered = true;
        _onHoverFn(this);
    }

    void Button::press() {
        setTexture(_buttonPressedSprite);
        if (_isPressed) {
            return;
        }
        _isPressed = true;
        physicsInfo.position.y += _pressOffset;
        _onPressFn(this);
    }

    void Button::release() {
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
