#pragma once
#include "Entity.h"
#include "Game.h"

namespace antibody {
    class Antibody;
}

namespace worker {
    class Worker final : public entity::Entity {
    public:

        Worker();

        Worker(const Worker& other);

        void initialize() override;

        void corrupt();

        bool isCorrupted() const;

        void purify();

        void registerAntibody(antibody::Antibody* antibody);

        void applyAntibodyEffects(float maxSpeedMultiplier);

    private:
        bool _isLoaded;
        bool _isAtTargetLocation;
        bool _isCorrupted;
        bool _isPurifying;
        bool _isIdle;
        slurp::Vec2<float> _targetLocation;
        types::set_arena<antibody::Antibody*> _attachedAntibodies;

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
