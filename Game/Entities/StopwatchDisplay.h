#pragma once
#include "Entity.h"
#include "NumberDisplay.h"

namespace ui {
    class StopwatchDisplay final : public entity::Entity {
    public:
        StopwatchDisplay() = default;

        StopwatchDisplay(const slurp::Vec2<float>& position);

        void start();

        void stop();

        void reset();

    private:
        bool _isStopped;
        float _secondsElapsed;

        NumberDisplay _hoursDisplay;
        NumberDisplay _minutesDisplay;
        NumberDisplay _secondsDisplay;
        NumberDisplay _deciSecondsDisplay;

        void initialize() override;

        void update(float dt) override;
    };
}
