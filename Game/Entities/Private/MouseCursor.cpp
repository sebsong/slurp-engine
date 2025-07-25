#include "MouseCursor.h"

#include "Game.h"

namespace game {
    static const std::string MouseCursorSpriteFileName = "mouse_cursor.bmp";
    static const render::Sprite MouseCursorSprite = render::loadSprite(MouseCursorSpriteFileName);

    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            MouseCursorSprite,
            true
        ),
        physics::PhysicsInfo(slurp::Vector2<int>::Zero),
        collision::CollisionInfo()
    ) {}

    void MouseCursor::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);
        this->physicsInfo.position = mouseState.position;
    }
}
