#include "Player.h"

#include "Entity.h"
#include "Geometry.h"
#include "Collision.h"

namespace player {
    static const slurp::Vector2 StartPosition = {640, 360};
    static constexpr int BaseSpeed = 400;
    static constexpr int SprintSpeed = 800;
    static constexpr int ShapeSize = 16a;
    static const geometry::Shape Shape = {
        geometry::Rect,
        {ShapeSize, ShapeSize}
    };
    static constexpr const char* Name = "Player";
    static constexpr const char* SpriteFileName = "player.bmp";
    static constexpr const char* ParrySpriteFileName = "player_parry.bmp";
    static const render::Sprite Sprite = render::loadSprite(SpriteFileName);
    static const render::Sprite ParrySprite = render::loadSprite(ParrySpriteFileName);

    static const timer::timer_handle ParryTimerHandle = timer::getNewHandle();
    static constexpr float ParryActiveDuration = .2f;
    static constexpr float ProjectileSpawnOffset = 10.f;

    Player::Player()
        : Entity(
              Name,
              render::RenderInfo(Sprite, true),
              physics::PhysicsInfo(
                  StartPosition,
                  BaseSpeed
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

        if (keyboardState.justPressed(slurp::KeyboardCode::SPACE)) { this->physicsInfo.speed = SprintSpeed; } else if (
            keyboardState.justReleased(slurp::KeyboardCode::SPACE)) { this->physicsInfo.speed = BaseSpeed; }

        if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
            projectile::Projectile& projectile = game::GlobalGameState->projectiles[game::GlobalGameState->
                projectileIdx];
            const slurp::Vector2<float> direction =
                    static_cast<slurp::Vector2<float>>(mouseState.position - this->physicsInfo.position).
                    normalize();
            const slurp::Vector2<int> position = this->physicsInfo.position + direction * ProjectileSpawnOffset;
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
        ) { this->physicsInfo.speed = SprintSpeed; } else if (
            gamepadState.justReleased(slurp::GamepadCode::LEFT_SHOULDER) ||
            gamepadState.justReleased(slurp::GamepadCode::RIGHT_SHOULDER)
        ) { this->physicsInfo.speed = BaseSpeed; }

        slurp::Vector2<float> leftStick = gamepadState.leftStick.end;
        slurp::Vector2<float> direction = leftStick;
        direction.y *= -1;
        this->physicsInfo.direction = direction.normalize();

        float leftTrigger = gamepadState.leftTrigger.end;
        float rightTrigger = gamepadState.rightTrigger.end;
        game::GlobalPlatformDll->vibrateGamepad(gamepadIndex, leftTrigger, rightTrigger);
    }

    void Player::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);
    }

    void Player::onCollisionExit(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionExit(collisionDetails);
    }

    void Player::activateParry() {
        this->isParryActive = true;
        this->renderInfo.sprite = ParrySprite;
        timer::start(ParryTimerHandle, ParryActiveDuration, false, [this] { deactivateParry(); });
    }

    void Player::deactivateParry() {
        this->isParryActive = false;
        this->renderInfo.sprite = Sprite;
    }
}
