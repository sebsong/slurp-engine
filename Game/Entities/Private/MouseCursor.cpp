#include "MouseCursor.h"

#include "Game.h"

namespace mouse_cursor {
    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            game::GlobalGameAssets->mouseCursorSprite,
            true
        ),
        physics::PhysicsInfo(slurp::Vec2<float>::Zero),
        collision::CollisionInfo()
    ) {}

    enemy::Enemy* MouseCursor::getClosestEnemy() const {
        enemy::Enemy* closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();
        for (enemy::Enemy& enemy: game::GlobalGameState->enemies) {
            float distance = enemy.physicsInfo.position.distanceSquaredTo(this->physicsInfo.position);
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
