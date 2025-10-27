#pragma once
#include "Entity.h"

namespace worker {
    class Worker final : public entity::Entity {
    public:
        Worker();

        Worker(const Worker& other);

    private:
        bool _isLoaded;

        void initialize() override;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        // void onCollisionExit(const collision::CollisionDetails& collisionDetails) override {};
    };
}
