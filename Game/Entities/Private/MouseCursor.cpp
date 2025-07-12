#include "MouseCursor.h"

#include "Game.h"

namespace game {
    static constexpr int MouseCursorSizePixels = 10;
    static constexpr render::ColorPaletteIdx MouseCursorColorPalletIdx = 1;

    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        {geometry::Rect, {MouseCursorSizePixels, MouseCursorSizePixels}},
        true,
        getColor(MouseCursorColorPalletIdx),
        {}
    ) {}

    void MouseCursor::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);
        this->position = mouseState.position;
    }
}
