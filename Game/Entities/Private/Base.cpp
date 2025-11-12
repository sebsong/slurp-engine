#include "Base.h"

#include "Turret.h"

namespace base {
    static const geometry::Shape BaseShape = {geometry::Rect, {32, 10}};
    static const slurp::Vec2<float> RenderOffset = {0, 8};
    static slurp::Vec2<float> SpawnOffset = {-15, -7};

    static constexpr uint32_t InitialGold = 100;
    static constexpr uint32_t GoldGoal = 1000;
    static const char* ProgressUniformName = "progress";

    Base::Base(): Entity(
                      "Base",
                      render::RenderInfo(slurp::Globals->GameAssets->baseSprite, true, 0, RenderOffset),
                      physics::PhysicsInfo(),
                      collision::CollisionInfo()
                  ),
                  _gold(InitialGold) {}

    void Base::spawnWorker() const {
        game::State->workers.newInstance(physicsInfo.position + SpawnOffset);
    }

    slurp::Vec2<float> Base::getDropOffLocation() const {
        return physicsInfo.position + SpawnOffset;
    }

    void Base::dropOff() {
        _gold++;
        game::State->spawnControls.refresh();
    }

    float Base::getProgress() const {
        return static_cast<float>(_gold) / GoldGoal;
    }

    bool Base::canSpend(uint32_t amount) const {
        return _gold >= amount;
    }

    void Base::spend(uint32_t amount) {
        if (canSpend(amount)) {
            _gold -= amount;
            game::State->spawnControls.refresh();
        }
    }

    void Base::initialize() {
        Entity::initialize();
    }

    void Base::update(float dt) {
        Entity::update(dt);

        game::State->resourcesCollectedDisplay.number = _gold;

        // debug::drawPoint(getDropOffLocation(), 4, DEBUG_GREEN_COLOR);
    }


    void Base::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (keyboardState.justPressed(slurp::KeyboardCode::NUM_3)) {
            turret::Turret* turret = game::State->turrets.nextInstance();
            turret->physicsInfo.position = mouseState.position;
            game::State->turrets.enableInstance(turret);
        }
    }
}
