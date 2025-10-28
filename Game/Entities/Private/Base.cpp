#include "Base.h"

namespace base {
    static const geometry::Shape BaseShape = {geometry::Rect, {32, 10}};
    static const slurp::Vec2<float> RenderOffset = {0, 3};
    static slurp::Vec2<float> WorkerSpawnOffset = {0, -5};

    Base::Base(): Entity(
        "Base",
        render::RenderInfo(slurp::Globals->GameAssets->baseSprite, true, 0, RenderOffset),
        physics::PhysicsInfo(),
        collision::CollisionInfo(
            true,
            true,
            BaseShape,
            true
        )
    ) {}

    slurp::Vec2<float> Base::getDropOffLocation() const {
        return physicsInfo.position + WorkerSpawnOffset;
    }

    void Base::initialize() {
        Entity::initialize();
    }

    void Base::update(float dt) {
        Entity::update(dt);
        debug::drawPoint(getDropOffLocation(), 4, DEBUG_GREEN_COLOR);
    }

    void Base::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (keyboardState.justPressed(slurp::KeyboardCode::NUM_1)) {
            worker::Worker& newWorker = game::State->workers.getNext();
            newWorker.physicsInfo.position = physicsInfo.position + WorkerSpawnOffset;
            newWorker.enabled = true;
        }
    }
}
