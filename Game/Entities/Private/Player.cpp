#include "Player.h"

#include "Entity.h"
#include "Geometry.h"
#include "Collision.h"

namespace player {
    static const slurp::Vec2 StartPosition = {0, 0};
    static constexpr float BaseSpeed = 300;
    static constexpr float SprintSpeed = 500;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static constexpr int ShapeSize = 16;
    static const geometry::Shape Shape = {
        geometry::Rect,
        {ShapeSize, ShapeSize}
    };
    static constexpr const char* Name = "Player";
    static constexpr const char* SpriteFileName = "player.bmp";
    static constexpr const char* ParrySpriteFileName = "player_parry.bmp";

    static const timer::timer_handle ParryTimerHandle = timer::getNewHandle();
    static constexpr float ParryActiveDuration = .2f;
    static constexpr float ProjectileSpawnOffset = 10.f;

    Player::Player()
        : Entity(
              Name,
              render::RenderInfo(game::GlobalGameAssets->playerSprite, true),
              physics::PhysicsInfo(
                  StartPosition,
                  BaseSpeed,
                  BaseAcceleration
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

        slurp::Vec2<float> directionUpdate{};
        if (keyboardState.isDown(slurp::KeyboardCode::W)) { directionUpdate.y += 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::A)) { directionUpdate.x -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::S)) { directionUpdate.y -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::D)) { directionUpdate.x += 1; }
        if (!directionUpdate.isZero()) {
            this->physicsInfo.direction = directionUpdate;
            this->physicsInfo.acceleration = BaseAcceleration;
        } else {
            this->physicsInfo.acceleration = -BaseAcceleration;
        }
        this->physicsInfo.direction.normalize();

        if (keyboardState.justPressed(slurp::KeyboardCode::SPACE)) { this->physicsInfo.maxSpeed = SprintSpeed; } else if
        (
            keyboardState.justReleased(slurp::KeyboardCode::SPACE)) { this->physicsInfo.maxSpeed = BaseSpeed; }

        if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
            projectile::Projectile& projectile = game::GlobalGameState->projectiles[game::GlobalGameState->
                projectileIdx];
            const slurp::Vec2<float> direction = (mouseState.position - this->physicsInfo.position).normalize();
            const slurp::Vec2<float> position = this->physicsInfo.position + direction * ProjectileSpawnOffset;
            projectile.fire(position);
        }

        if (
            mouseState.justPressed(slurp::MouseCode::RightClick) ||
            keyboardState.justPressed(slurp::KeyboardCode::E)
        ) {
            activateParry();
        }
    }

    void Player::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);

        if (
            gamepadState.justPressed(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justPressed(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->physicsInfo.maxSpeed = SprintSpeed; } else if (
            gamepadState.justReleased(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justReleased(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->physicsInfo.maxSpeed = BaseSpeed; }

        slurp::Vec2<float> leftStick = gamepadState.leftStick.end;
        slurp::Vec2<float> direction = leftStick;
        direction.y *= -1;
        this->physicsInfo.direction = direction.normalize();

        float leftTrigger = gamepadState.leftTrigger.end;
        float rightTrigger = gamepadState.rightTrigger.end;
        slurp::GlobalPlatformDll->vibrateGamepad(gamepadIndex, leftTrigger, rightTrigger);
    }

    void Player::update(float dt) {
        Entity::update(dt);
    }

    void Player::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);
    }

    void Player::onCollisionExit(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionExit(collisionDetails);
    }

    void Player::activateParry() {
        this->isParryActive = true;
        this->renderInfo.sprite = game::GlobalGameAssets->playerParrySprite;
        timer::start(ParryTimerHandle, ParryActiveDuration, false, [this] { deactivateParry(); });
    }

    void Player::deactivateParry() {
        this->isParryActive = false;
        this->renderInfo.sprite = game::GlobalGameAssets->playerSprite;
    }
}
