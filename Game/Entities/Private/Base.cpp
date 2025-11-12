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
                  gold(InitialGold) {}

    void Base::spawnWorker() const {
        game::State->workers.newInstance(physicsInfo.position + SpawnOffset);
    }

    slurp::Vec2<float> Base::getDropOffLocation() const {
        return physicsInfo.position + SpawnOffset;
    }

    void Base::dropOff() {
        gold++;
    }

    float Base::getProgress() const {
        return static_cast<float>(gold) / GoldGoal;
    }

    void Base::initialize() {
        Entity::initialize();
    }

    void Base::update(float dt) {
        Entity::update(dt);

        game::State->resourcesCollectedDisplay.number = gold;

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
