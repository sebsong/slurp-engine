#pragma once
#include "Entity.h"

namespace worker {
    class Worker final : public entity::Entity {
    public:
        Worker();

        Worker(const Worker& other);

        void initialize() override;

        void infect();

    private:
        bool _isLoaded;
        bool _isAtTargetLocation;
        bool _isInfected;
        slurp::Vec2<float> _targetLocation;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void setTargetLocation(slurp::Vec2<float> newTargetLocation);

        void beginDropOff();

        void dropOff();

        void playDropOffAnim();

        void beginCollect();

        void collect();

        void playCollectionAnim();
    };
}
