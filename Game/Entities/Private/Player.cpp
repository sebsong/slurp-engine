#include "Player.h"

#include "Game.h"
#include "Entity.h"
#include "Geometry.h"
#include "Render.h"
#include "Collision.h"

#include <iostream>

namespace game {
    static const slurp::Vector2 PlayerStartPos = {640, 360};
    static constexpr int BasePlayerSizePixels = 20;
    static constexpr render::ColorPaletteIdx PlayerColorPalletIdx = 3;
    static constexpr render::ColorPaletteIdx PlayerParryColorPalletIdx = 0;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;
    static const geometry::Shape Shape = {
        geometry::Rect,
        {BasePlayerSizePixels, BasePlayerSizePixels}
    };
    static constexpr const char* Name = "Player";

    static constexpr float ParryActiveDuration = .1f;

    Player::Player()
        : Entity(
              Name,
              Shape,
              true,
              getColor(PlayerColorPalletIdx),
              PlayerStartPos,
              BasePlayerSpeed,
              collision::CollisionInfo(
                  false,
                  Shape,
                  true
              )
          ),
          isParryActive(false) {}

    void Player::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        slurp::Vector2<float> dir;
        if (keyboardState.isDown(slurp::KeyboardCode::W)) { dir.y -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::A)) { dir.x -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::S)) { dir.y += 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::D)) { dir.x += 1; }
        this->direction = dir.normalize();

        if (keyboardState.justPressed(slurp::KeyboardCode::SPACE)) { this->speed = SprintPlayerSpeed; }
        else if (keyboardState.justReleased(slurp::KeyboardCode::SPACE)) { this->speed = BasePlayerSpeed; }

        if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.position;
            projectile.direction =
                    static_cast<slurp::Vector2<float>>(mouseState.position - GlobalGameState->player.position).
                    normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) { GlobalGameState->projectileIdx = 0; }
        }

        if (
            mouseState.justPressed(slurp::MouseCode::RightClick) ||
            keyboardState.justPressed(slurp::KeyboardCode::E)
        ) {
            activateParry();
            timer::delay(ParryActiveDuration, [this] { deactivateParry(); });
        }
    }

    void Player::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (
            gamepadState.justPressed(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justPressed(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->speed = SprintPlayerSpeed; }
        else if (
            gamepadState.justReleased(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justReleased(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->speed = BasePlayerSpeed; }

        slurp::Vector2<float> leftStick = gamepadState.leftStick.end;
        slurp::Vector2<float> direction = leftStick;
        direction.y *= -1;
        this->direction = direction.normalize();

        float leftTrigger = gamepadState.leftTrigger.end;
        float rightTrigger = gamepadState.rightTrigger.end;
        GlobalPlatformDll->vibrateGamepad(gamepadIndex, leftTrigger, rightTrigger);
    }

    void Player::onCollisionEnter(const Entity* otherEntity) {
        std::cout << "ENTER: " << otherEntity->name << std::endl;
    }


    void Player::onCollisionExit(const Entity* otherEntity) { std::cout << "EXIT: " << otherEntity->name << std::endl; }

    void Player::activateParry() {
        this->isParryActive = true;
        this->renderShape.color = getColor(PlayerParryColorPalletIdx);
    }

    void Player::deactivateParry() {
        this->isParryActive = false;
        this->renderShape.color = getColor(PlayerColorPalletIdx);
    }
}
