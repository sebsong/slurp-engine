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
              "IntDisplay",
              render::RenderInfo(),
              physics::PhysicsInfo(position),
              collision::CollisionInfo()
          ),
          number(initialNumber),
          _numDigits(numDigits),
          _showLeadingZeroes(showLeadingZeroes),
          _digitDisplays({}) {
        for (int i = 0; i < numDigits; ++i) {
            _digitDisplays[i] = Entity(
                std::format("Digit {}", i),
                render::RenderInfo(game::Assets->digitSprites[0], true, game::UI_Z),
                physics::PhysicsInfo({physicsInfo.position.x - i * 10, physicsInfo.position.y}),
                collision::CollisionInfo()
            );
            _digitDisplays[i].renderInfo.sprite = nullptr;
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
                _digitDisplays[i].renderInfo.sprite = nullptr;
                return;
            }
            uint8_t digit = shiftedNum % 10;
            _digitDisplays[i].renderInfo.sprite = game::Assets->digitSprites[digit];
        }
    }
}
