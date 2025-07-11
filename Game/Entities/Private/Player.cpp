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

    void Player::handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
    ) {
        slurp::Vector2<float> dir;
        if (keyboardState.isDown(slurp::KeyboardCode::W)) {
            dir.y -= 1;
        }
        if (keyboardState.isDown(slurp::KeyboardCode::A)) {
            dir.x -= 1;
        }
        if (keyboardState.isDown(slurp::KeyboardCode::S)) {
            dir.y += 1;
        }
        if (keyboardState.isDown(slurp::KeyboardCode::D)) {
            dir.x += 1;
        }
        this->direction = dir.normalize();


        if (keyboardState.justPressed(slurp::KeyboardCode::SPACE)) {
            this->speed = SprintPlayerSpeed;
        } else if (keyboardState.justReleased(slurp::KeyboardCode::SPACE)) {
            this->speed = BasePlayerSpeed;
        }
    }

    void Player::onCollisionEnter(const Entity* otherEntity) {
        std::cout << "ENTER: " << otherEntity->name << std::endl;
    }


    void Player::onCollisionExit(const Entity* otherEntity) {
        std::cout << "EXIT: " << otherEntity->name << std::endl;
    }
}
