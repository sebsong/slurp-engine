#include "Worker.h"

namespace worker {
    static const geometry::Shape WorkerShape = {geometry::Rect, {5, 2}};
    static constexpr float BaseSpeed = 100;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static const slurp::Vec2<float> StartPos = {50, 50};

    Worker::Worker(): Entity(
        "Worker",
        render::RenderInfo(
            slurp::Globals->GameAssets->workerSprite,
            true,
            StartPos.y,
            {0, 2}
        ),
        physics::PhysicsInfo(
            StartPos,
            BaseSpeed,
            BaseAcceleration
        ),
        collision::CollisionInfo(
            false,
            false,
            WorkerShape,
            true
        )
    ) {}

    void Worker::initialize() {
        Entity::initialize();
    }

    void Worker::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);
        slurp::Vec2<float> directionUpdate{};
        if (keyboardState.isDown(slurp::KeyboardCode::W)) { directionUpdate.y += 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::A)) { directionUpdate.x -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::S)) { directionUpdate.y -= 1; }
        if (keyboardState.isDown(slurp::KeyboardCode::D)) { directionUpdate.x += 1; }
        if (!directionUpdate.isZero()) {
            this->physicsInfo.direction = directionUpdate;
            this->physicsInfo.acceleration = BaseAcceleration;
        } else {
            this->physicsInfo.acceleration = -BaseAcceleration;
        }
        this->physicsInfo.direction.normalize();
    }

    void Worker::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);
    }

    void Worker::update(float dt) {
        Entity::update(dt);
        renderInfo.zOrder = physicsInfo.position.y;
    }
}
