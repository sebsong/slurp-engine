#pragma once
#include "Entity.h"

namespace worker {
    class Worker final : public entity::Entity {
    public:

        Worker();

        void initialize() override;

        void corrupt();

        bool isCorrupted() const;

        void decrementCorruption();

    private:
        bool _isLoaded;
        bool _isAtTargetLocation;
        bool _isCorrupted;
        bool _isPurifying;
        bool _isIdle;
        slurp::Vec2<float> _targetLocation;
        int _corruptionRemaining;

        void update(float dt) override;

        void setTargetLocation(slurp::Vec2<float> newTargetLocation);

        void occupyMiningLocation(slurp::Vec2<float> location);

        void leaveMiningLocation();

        void findNewMiningLocation();

        void beginDropOff();

        void dropOff();

        void beginCollect();

        void collect();

        void idle();

        void purify();

        void erupt();
    };
}
