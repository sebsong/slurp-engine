#pragma once
#include "Entity.h"
#include "NumberDisplay.h"

namespace ui {
    class Stopwatch final : public entity::Entity {
    public:
        Stopwatch() = default;

        explicit Stopwatch(const slurp::Vec2<float>& position);

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
