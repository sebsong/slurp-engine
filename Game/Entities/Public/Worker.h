#pragma once
#include "Entity.h"

namespace worker {
    class Worker final : public entity::Entity {
    public:

        Worker();

        void initialize() override;

        void corrupt();

        bool isCorrupted() const;

        void purify();

    private:
        bool _isLoaded;
        bool _isAtTargetLocation;
        bool _isCorrupted;
        bool _isPurifying;
        bool _isIdle;
        slurp::Vec2<float> _targetLocation;
        uint8_t _corruptionRemaining;

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;

        void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) override;

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void setTargetLocation(slurp::Vec2<float> newTargetLocation);

        void occupyMiningLocation(slurp::Vec2<float> location);

        void leaveMiningLocation();

        void findNewMiningLocation();

        void beginDropOff();

        void dropOff();

        void beginCollect();

        void collect();

        void idle();
    };
}
