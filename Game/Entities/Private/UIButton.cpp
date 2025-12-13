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
        std::function<void(UIButton* button)>&& onReleaseFn
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
        _buttonDisabled(false),
        _onPressFn(std::move(onPressFn)),
        _onReleaseFn(std::move(onReleaseFn)),
        _keyCode(keyCode),
        _buttonSprite(buttonSprite),
        _buttonHoverSprite(buttonHoverSprite),
        _buttonPressedSprite(buttonPressSprite) {}

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
        _onPressFn(this);
    }

    void UIButton::release() {
        setTexture(_buttonSprite);
        if (!_isPressed) {
            return;
        }
        _isPressed = false;
        _onReleaseFn(this);
    }
}
