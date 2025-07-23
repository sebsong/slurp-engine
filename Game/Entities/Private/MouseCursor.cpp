#include "MouseCursor.h"

#include "Game.h"

namespace game {
    static constexpr int MouseCursorSizePixels = 10;
    static constexpr render::ColorPaletteIdx MouseCursorColorPalletIdx = 1;

    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            render::RenderShape{
                {geometry::Rect, {MouseCursorSizePixels, MouseCursorSizePixels}},
                getColor(MouseCursorColorPalletIdx),
            },
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
