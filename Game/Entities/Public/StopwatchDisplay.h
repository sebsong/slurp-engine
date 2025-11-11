#pragma once
#include "Entity.h"

namespace ui {
    class StopwatchDisplay final : public entity::Entity {
    public:
        float secondsElapsed;

        StopwatchDisplay(const slurp::Vec2<float>& position);

        void start();

        void stop();

        void reset();

    private:
        bool _isStopped;

        NumberDisplay _hoursDisplay;
        NumberDisplay _minutesDisplay;
        NumberDisplay _secondsDisplay;
        NumberDisplay _deciSecondsDisplay;
        Entity _punctuationDisplay;

        void initialize() override;

        void update(float dt) override;
    };
}
