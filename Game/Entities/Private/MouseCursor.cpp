#include "MouseCursor.h"

#include "Game.h"

namespace mouse_cursor {
    static const std::string SpriteFileName = "mouse_cursor.bmp";
    static const render::Sprite Sprite = render::loadSprite(SpriteFileName);

    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            Sprite,
            true
        ),
        physics::PhysicsInfo(slurp::Vector2<int>::Zero),
        collision::CollisionInfo()
    ) {}

    enemy::Enemy* MouseCursor::getClosestEnemy() const {
        enemy::Enemy* closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();
        for (enemy::Enemy& enemy : game::GlobalGameState->enemies) {
            float distance = enemy.physicsInfo.position.distanceTo(this->physicsInfo.position);
            if (distance < closestDistance) {
                closestEnemy = &enemy;
                closestDistance = distance;
            }
        }

        return closestEnemy;
    }

    void MouseCursor::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);
        this->physicsInfo.position = mouseState.position;
    }
}
