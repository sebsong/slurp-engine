#pragma once
#include "Entity.h"

namespace game {
    class Enemy;

    class MouseCursor final : public slurp::Entity {
    public:
        MouseCursor();

        Enemy* getClosestEnemy() const;

    private:
        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
    };
}
