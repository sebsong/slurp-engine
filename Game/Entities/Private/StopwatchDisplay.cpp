#include "StopwatchDisplay.h"

namespace ui {
    StopwatchDisplay::StopwatchDisplay(const slurp::Vec2<float>& position)
        : Entity("TimeDisplay"),
          _isStopped(true),
          _hoursDisplay(
              NumberDisplay(
                  {position.x - 60, position.y},
                  0,
                  2,
                  true
              )
          ),
          _minutesDisplay(
              NumberDisplay(
                  {position.x - 30, position.y},
                  0,
                  2,
                  true
              )
          ),
          _secondsDisplay(
              NumberDisplay(
                  {position.x, position.y},
                  0,
                  2,
                  true
              )
          ),
          _millisDisplay(
              NumberDisplay(
                  {position.x + 40, position.y},
                  0,
                  3,
                  true
              )
          ) {
        entity::registerEntity(_hoursDisplay);
        entity::registerEntity(_minutesDisplay);
        entity::registerEntity(_secondsDisplay);
        entity::registerEntity(_millisDisplay);
    }

    void StopwatchDisplay::start() {
        _isStopped = false;
    }

    void StopwatchDisplay::stop() {
        _isStopped = true;
    }

    void StopwatchDisplay::reset() {
        secondsElapsed = 0;
    }

    void StopwatchDisplay::initialize() {
        Entity::initialize();
    }

    void StopwatchDisplay::update(float dt) {
        Entity::update(dt);
        secondsElapsed += dt;

        uint32_t hours = secondsElapsed / (60 * 60);
        uint32_t minutes = static_cast<uint32_t>(secondsElapsed / 60) % 60;
        uint32_t seconds = static_cast<uint32_t>(secondsElapsed) % 60;
        uint32_t millis = (secondsElapsed - static_cast<uint32_t>(secondsElapsed)) * 1000;
        _hoursDisplay.number = hours;
        _minutesDisplay.number = minutes;
        _secondsDisplay.number = seconds;
        _millisDisplay.number = millis;
    }
}
