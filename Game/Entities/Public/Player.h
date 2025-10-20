#pragma once

#include "Entity.h"
#include "Input.h"
#include "Timer.h"

namespace player {
    class Player final : public slurp::Entity {
    public:
        Player();

        bool isParryActive;

    private:
        timer::timer_handle _parryTimerHandle;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void onCollisionExit(const collision::CollisionDetails& collisionDetails) override;

        void activateParry();

        void deactivateParry();
    };
}
