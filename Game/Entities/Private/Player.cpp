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
    static constexpr const char* SpriteFileName = "player.bmp";

    static constexpr float ParryActiveDuration = .1f;
    static constexpr float ProjectileSpawnOffset = 10.f;

    Player::Player()
        : Entity(
              Name,
              render::RenderInfo(SpriteFileName, true),
              physics::PhysicsInfo(
                  PlayerStartPos,
                  BasePlayerSpeed
              ),
              collision::CollisionInfo(
                  false,
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
        this->physicsInfo.direction = dir.normalize();

        if (keyboardState.justPressed(slurp::KeyboardCode::SPACE)) { this->physicsInfo.speed = SprintPlayerSpeed; }
        else if (keyboardState.justReleased(slurp::KeyboardCode::SPACE)) { this->physicsInfo.speed = BasePlayerSpeed; }

        if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
            Projectile& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            const slurp::Vector2<float> direction =
                    static_cast<slurp::Vector2<float>>(mouseState.position - this->physicsInfo.position).
                    normalize();
            const slurp::Vector2<int> position = this->physicsInfo.position + direction * ProjectileSpawnOffset;
            projectile.fire(position, direction);
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
        ) { this->physicsInfo.speed = SprintPlayerSpeed; }
        else if (
            gamepadState.justReleased(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justReleased(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->physicsInfo.speed = BasePlayerSpeed; }

        slurp::Vector2<float> leftStick = gamepadState.leftStick.end;
        slurp::Vector2<float> direction = leftStick;
        direction.y *= -1;
        this->physicsInfo.direction = direction.normalize();

        float leftTrigger = gamepadState.leftTrigger.end;
        float rightTrigger = gamepadState.rightTrigger.end;
        GlobalPlatformDll->vibrateGamepad(gamepadIndex, leftTrigger, rightTrigger);
    }

    void Player::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);
        std::cout << "ENTER: " << collisionDetails.entity->name << std::endl;
    }


    void Player::onCollisionExit(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionExit(collisionDetails);
        std::cout << "EXIT: " << collisionDetails.entity->name << std::endl;
    }

    void Player::activateParry() {
        this->isParryActive = true;
        this->renderInfo.renderShape.color = getColor(PlayerParryColorPalletIdx);
    }

    void Player::deactivateParry() {
        this->isParryActive = false;
        this->renderInfo.renderShape.color = getColor(PlayerColorPalletIdx);
    }
}
