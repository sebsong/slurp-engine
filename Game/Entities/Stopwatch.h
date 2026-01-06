#pragma once
#include "Entity.h"
#include "NumberDisplay.h"

namespace ui {
    class Stopwatch final : public entity::Entity {
    public:
        Stopwatch() = default;

        explicit Stopwatch(
            const slurp::Vec2<float>& position,
            int32_t zOrder = game::UI_Z,
            float secondsElapsed = 0.f
        );

        void start();

        void stop();

        void reset();

        float getSecondsElapsed();

        void setSecondsElapsed(float secondsElapsed);

        void setColor(slurp::Vec4<float> color);

    private:
        bool _isStopped;
        float _secondsElapsed;

        NumberDisplay _hoursDisplay;
        NumberDisplay _minutesDisplay;
        NumberDisplay _secondsDisplay;
        NumberDisplay _deciSecondsDisplay;

        void initialize() override;

        void update(float dt) override;

        void updateDisplay();
    };
}
