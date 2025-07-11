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
        : slurp::Entity(
              Name,
              Shape,
              true,
              getColor(PlayerColorPalletIdx),
              PlayerStartPos,
              BasePlayerSpeed,
              // collision::CollisionInfo(),
              collision::CollisionInfo(
                  false,
                  Shape,
                  true,
                  [this](const slurp::Entity* otherEntity) { this->onCollisionEnter(otherEntity); },
                  [this](const slurp::Entity* otherEntity) { this->onCollisionExit(otherEntity); }
              ),
              [this](
          const slurp::MouseState& mouseState,
          const slurp::KeyboardState& keyboardState,
          const slurp::GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
      ) {
                  this->handleInput(mouseState, keyboardState, controllerStates);
              }
          ),
          isParryActive(false) {}

    void Player::handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
    ) {}

    void Player::onCollisionEnter(const slurp::Entity* otherEntity) {
        std::cout << "ENTER: " << otherEntity->name << std::endl;
    }


    void Player::onCollisionExit(const slurp::Entity* otherEntity) {
        std::cout << "EXIT: " << otherEntity->name << std::endl;
    }
}
