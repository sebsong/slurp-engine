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
          _display(
              entity::EntityPool<Entity, MAX_NUM_DIGITS>(
                  Entity(
                      "Digit",
                      render::RenderInfo(game::Assets->digitSprites[0], true, game::UI_Z),
                      physics::PhysicsInfo(),
                      collision::CollisionInfo()
                  )
              )
          ) {}

    void NumberDisplay::initialize() {
        Entity::initialize();
        _display.initialize();
        for (int i = 0; i < MAX_NUM_DIGITS; ++i) {
            slurp::Vec2<float> digitPosition = physicsInfo.position;
            digitPosition.x -= i * 10;
            _display.newInstance(digitPosition)->renderInfo.sprite = nullptr;
        }
    }


    static void displayInt(
        int num,
        uint8_t numDigits,
        bool showLeadingZeroes,
        entity::EntityPool<entity::Entity, MAX_NUM_DIGITS>& displayEntity
    ) {
        for (int i = 0; i < numDigits; i++) {
            uint32_t shiftedNum = num / std::pow(10, i);
            if (!showLeadingZeroes && shiftedNum == 0 && i != 0) {
                return;
            }
            uint8_t digit = shiftedNum % 10;
            displayEntity[i]->renderInfo.sprite = game::Assets->digitSprites[digit];
        }
    }

    void NumberDisplay::update(float dt) {
        Entity::update(dt);
        displayInt(number, _numDigits, _showLeadingZeroes, _display);
    }
}
