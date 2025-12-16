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
        slurp::KeyboardCode keyCode,
        std::function<void(UIButton* button)>&& onPressFn,
        std::function<void(UIButton* button)>&& releaseActionFn,
        std::function<void(UIButton* button)>&& onReleaseFn,
        float pressOffset
    ) : Entity(
            "UI Button",
            render::RenderInfo(
                (asset::SpriteInstance[2]){
                    asset::SpriteInstance(
                        buttonSprite,
                        game::UI_Z,
                        buttonRenderOffset
                    ),
                    asset::SpriteInstance(
                        buttonIconSprite,
                        game::UI_Z - 1,
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
        _buttonDisabled(false),
        _onPressFn(std::move(onPressFn)),
        _releaseActionFn(std::move(releaseActionFn)),
        _onReleaseFn(std::move(onReleaseFn)),
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

        bool pressedByKey = keyboardState.isDown(_keyCode);
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
        }
        release();

        if (mouseOnButton) {
            hover();
        }
    }

    void UIButton::hover() {
        setTexture(_buttonHoverSprite);
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
        if (!_isPressed) {
            return;
        }
        _wasPressedByMouse = false;
        _isPressed = false;
        physicsInfo.position.y -= _pressOffset;
    }
}
