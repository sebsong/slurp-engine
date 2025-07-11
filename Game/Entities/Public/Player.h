#pragma once

#include "Entity.h"
#include "Input.h"

namespace game {
    class Player final : public slurp::Entity {
    public:
        Player();

        bool isParryActive;

    private:
        void handleInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState,
            const slurp::GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
        );

        void onCollisionEnter(const slurp::Entity* other);

        void onCollisionExit(const slurp::Entity* other);
    };
}
