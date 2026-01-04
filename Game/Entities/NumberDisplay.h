#pragma once
#include "Entity.h"
#include "ZOrder.h"

#define MAX_NUM_DIGITS 9

namespace ui {
    class NumberDisplay final : public entity::Entity {
    public:
        int number;

        NumberDisplay() = default;

        NumberDisplay(
            slurp::Vec2<float> position,
            int initialNumber,
            uint8_t numDigits,
            bool showLeadingZeroes,
            int32_t zOrder = game::UI_Z
        );

    private:
        uint8_t _numDigits;
        bool _showLeadingZeroes;

        void initialize() override;

        void update(float dt) override;
    };
}
