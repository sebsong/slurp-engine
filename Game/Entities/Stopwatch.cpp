#include "Stopwatch.h"

#include "Game.h"
#include "NumberDisplay.h"

namespace ui {
    Stopwatch::Stopwatch(
        const slurp::Vec2<float>& position,
        int32_t zOrder,
        float secondsElapsed
    ): Entity(
           "TimeDisplay",
           render::RenderInfo(
               render::SpriteInstance(
                   game::Assets->stopwatchPunctuationSprite,
                   zOrder,
                   {-16, 0}
               )
           ),
           physics::PhysicsInfo(position),
           collision::CollisionInfo()
       ),
       _isStopped(true),
       _secondsElapsed(secondsElapsed),
       _hoursDisplay(
           NumberDisplay(
               {position.x - 44, position.y},
               0,
               2,
               true,
               zOrder
           )
       ),
       _minutesDisplay(
           NumberDisplay(
               {position.x - 22, position.y},
               0,
               2,
               true,
               zOrder
           )
       ),
       _secondsDisplay(
           NumberDisplay(
               {position.x, position.y},
               0,
               2,
               true,
               zOrder
           )
       ),
       _deciSecondsDisplay(
           NumberDisplay(
               {position.x + 12, position.y},
               0,
               1,
               true,
               zOrder
           )
       ) {}

    void Stopwatch::initialize() {
        Entity::initialize();
        scene::registerEntity(scene, &_hoursDisplay);
        scene::registerEntity(scene, &_minutesDisplay);
        scene::registerEntity(scene, &_secondsDisplay);
        scene::registerEntity(scene, &_deciSecondsDisplay);

        updateDisplay();
    }

    void Stopwatch::start() {
        _isStopped = false;
    }

    void Stopwatch::stop() {
        _isStopped = true;
    }

    void Stopwatch::reset() {
        _secondsElapsed = 0;
    }

    float Stopwatch::getSecondsElapsed() {
        return _secondsElapsed;
    }

    void Stopwatch::setSecondsElapsed(float secondsElapsed) {
        _secondsElapsed = secondsElapsed;
        updateDisplay();
    }

    void Stopwatch::setColor(slurp::Vec4<float> color) {
        renderInfo.sprites[0].material.colorOverride = color;
        _hoursDisplay.setColor(color);
        _minutesDisplay.setColor(color);
        _secondsDisplay.setColor(color);
        _deciSecondsDisplay.setColor(color);
    }

    void Stopwatch::update(float dt) {
        Entity::update(dt);
        if (_isStopped) {
            return;
        }

        _secondsElapsed += dt;
        updateDisplay();
    }

    void Stopwatch::updateDisplay() {
        uint32_t hours = _secondsElapsed / (60 * 60);
        uint32_t minutes = static_cast<uint32_t>(_secondsElapsed / 60) % 60;
        uint32_t seconds = static_cast<uint32_t>(_secondsElapsed) % 60;
        uint32_t deciSeconds = (_secondsElapsed - static_cast<uint32_t>(_secondsElapsed)) * 10;

        _hoursDisplay.number = hours;
        _minutesDisplay.number = minutes;
        _secondsDisplay.number = seconds;
        _deciSecondsDisplay.number = deciSeconds;
    }
}
