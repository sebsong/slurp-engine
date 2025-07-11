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
        ) override;

        void onCollisionEnter(const Entity* other) override;

        void onCollisionExit(const Entity* other) override;
    };
}
