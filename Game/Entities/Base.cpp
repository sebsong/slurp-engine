#include "Base.h"

#include "Game.h"

namespace base {
    static const geometry::Shape BaseShape = {geometry::Rect, {32, 10}};
    static const slurp::Vec2<float> RenderOffset = {0, 8};
    static constexpr int NumSpawnLocations = 2;
    static slurp::Vec2<float> SpawnOffsets[NumSpawnLocations] = {{-15, -8}, {16, -8}};

    static constexpr uint32_t InitialGold = 100;
    static constexpr uint32_t GoldGoal = 500;
    static const char* ProgressUniformName = "progress";

    Base::Base()
        : Entity(
              "Base",
              render::RenderInfo(asset::SpriteInstance(game::Assets->baseSprite, RenderOffset)),
              physics::PhysicsInfo(),
              collision::CollisionInfo()
          ),
          _gold(InitialGold) {}

    void Base::spawnWorker() const {
        game::State->workers.newInstance(getRandomSpawnLocation());
    }

    bool Base::isSpawnLocation(const slurp::Vec2<float>& location) const {
        for (slurp::Vec2 spawnOffset: SpawnOffsets) {
            if (location == (physicsInfo.position + spawnOffset)) {
                return true;
            }
        }
        return false;
    }

    slurp::Vec2<float> Base::getRandomSpawnLocation() const {
        return physicsInfo.position + SpawnOffsets[rnd::randomIndex(NumSpawnLocations)];
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
    }
}
