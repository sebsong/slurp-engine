#include "NumberDisplay.h"

#include "Game.h"

namespace ui {
    NumberDisplay::NumberDisplay(
        slurp::Vec2<float> position,
        int initialNumber,
        uint8_t numDigits,
        bool showLeadingZeroes
    )
        : Entity(
              "Number Display",
              render::RenderInfo(
                  MAX_NUM_DIGITS,
                  nullptr
              ),
              physics::PhysicsInfo(position),
              collision::CollisionInfo()
          ),
          number(initialNumber),
          _numDigits(numDigits),
          _showLeadingZeroes(showLeadingZeroes) {
        for (int i = 0; i < MAX_NUM_DIGITS; ++i) {
            new(&renderInfo.sprites[i]) render::SpriteInstance(
                game::Assets->digitSprites[0],
                game::UI_Z,
                {-i * 10.f, 0.f}
            );
            renderInfo.sprites[i].renderingEnabled = false;
        }
    }

    void NumberDisplay::initialize() {
        Entity::initialize();
    }

    void NumberDisplay::update(float dt) {
        Entity::update(dt);
        for (int i = 0; i < _numDigits; i++) {
            uint32_t shiftedNum = number / std::pow(10, i);
            if (!_showLeadingZeroes && shiftedNum == 0 && i != 0) {
                renderInfo.sprites[i].renderingEnabled = false;
                return;
            }
            uint8_t digit = shiftedNum % 10;
            setTexture(i, game::Assets->digitSprites[digit]);
            renderInfo.sprites[i].renderingEnabled = true;
        }
    }
}
