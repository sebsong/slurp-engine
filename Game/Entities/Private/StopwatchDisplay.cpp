#include "StopwatchDisplay.h"

namespace ui {
    StopwatchDisplay::StopwatchDisplay(const slurp::Vec2<float>& position)
        : Entity(
              "TimeDisplay",
              render::RenderInfo(
                  asset::SpriteInstance(
                      game::Assets->stopwatchPunctuationSprite,
                      game::UI_Z,
                      {-16, 0}
                  )
              ),
              physics::PhysicsInfo(position),
              collision::CollisionInfo()
          ),
          _isStopped(true),
          _hoursDisplay(
              NumberDisplay(
                  {position.x - 44, position.y},
                  0,
                  2,
                  true
              )
          ),
          _minutesDisplay(
              NumberDisplay(
                  {position.x - 22, position.y},
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
          _deciSecondsDisplay(
              NumberDisplay(
                  {position.x + 12, position.y},
                  0,
                  1,
                  true
              )
          ) {}

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
        uint32_t deciSeconds = (secondsElapsed - static_cast<uint32_t>(secondsElapsed)) * 10;
        _hoursDisplay.number = hours;
        _minutesDisplay.number = minutes;
        _secondsDisplay.number = seconds;
        _deciSecondsDisplay.number = deciSeconds;
    }
}
