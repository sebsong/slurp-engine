#pragma once
#include "Entity.h"

namespace enemy {
    class Enemy;
}

namespace mouse_cursor {
    class MouseCursor final : public entity::Entity {
    public:
        MouseCursor() = default;

        MouseCursor(asset::Sprite* sprite);

    private:
        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
    };
}
