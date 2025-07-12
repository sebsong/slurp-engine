#pragma once
#include "Entity.h"

namespace game {
    class MouseCursor final : public slurp::Entity {
    public:
        MouseCursor();

    private:
        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
    };
}
