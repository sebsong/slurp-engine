#include "Stopwatch.h"

#include "Game.h"
#include "NumberDisplay.h"

namespace ui {
    Stopwatch::Stopwatch(const slurp::Vec2<float>& position, float secondsElapsed)
        : Entity(
              "TimeDisplay",
              render::RenderInfo(
                  render::SpriteInstance(
                      game::Assets->stopwatchPunctuationSprite,
                      game::UI_Z,
                      {-16, 0}
                  )
              ),
              physics::PhysicsInfo(position),
              collision::CollisionInfo()
          ),
          secondsElapsed(secondsElapsed),
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

    void Stopwatch::initialize() {
        Entity::initialize();
        scene::registerEntity(scene, &_hoursDisplay);
        scene::registerEntity(scene, &_minutesDisplay);
        scene::registerEntity(scene, &_secondsDisplay);
        scene::registerEntity(scene, &_deciSecondsDisplay);
    }

    void Stopwatch::start() {
        _isStopped = false;
    }

    void Stopwatch::stop() {
        _isStopped = true;
    }

    void Stopwatch::reset() {
        secondsElapsed = 0;
    }

    void Stopwatch::update(float dt) {
        Entity::update(dt);
        if (_isStopped) {
            return;
        }

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
