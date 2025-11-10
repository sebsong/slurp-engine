#pragma once
#include "Entity.h"
#include "EntityPool.h"

#define MAX_NUM_DIGITS 9

namespace ui {
    class NumberDisplay final : public entity::Entity {
    public:
        int number;

        NumberDisplay(
            slurp::Vec2<float> position,
            int initialNumber,
            uint8_t numDigits,
            bool showLeadingZeroes
        );

    private:
        uint8_t _numDigits;
        bool _showLeadingZeroes;
        Entity _digitDisplays[MAX_NUM_DIGITS];

        void initialize() override;

        void update(float dt) override;
    };
}
