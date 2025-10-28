#include "Worker.h"

#include "Game.h"

namespace worker {
    static const geometry::Shape WorkerShape = {geometry::Rect, {5, 2}};
    static constexpr float BaseSpeed = 100;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static const slurp::Vec2<float> StartPos = {50, 50};
    static const float CollectionTime = 2.f;
    static const float DropOffTime = .5f;

    Worker::Worker()
        : Entity(
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
                  true,
                  WorkerShape,
                  true
              )
          ),
          _isLoaded(false) {}

    Worker::Worker(const Worker& other): Entity(other) {}

    void Worker::initialize() {
        Entity::initialize();
        dropOff();
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
        // if (!directionUpdate.isZero()) {
        //     this->physicsInfo.direction = directionUpdate;
        //     this->physicsInfo.acceleration = BaseAcceleration;
        // } else {
        //     this->physicsInfo.acceleration = -BaseAcceleration;
        // }
        // this->physicsInfo.direction.normalize();
    }

    void Worker::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        Entity::handleGamepadInput(gamepadIndex, gamepadState);
    }

    static bool approxEqual(slurp::Vec2<float> a, slurp::Vec2<float> b) {
        return a.distanceSquaredTo(b) < 5.f;
    }

    void Worker::update(float dt) {
        Entity::update(dt);
        debug::drawPoint(physicsInfo.position, 4, DEBUG_RED_COLOR);
        renderInfo.zOrder = physicsInfo.position.y;

        if (approxEqual(physicsInfo.position, _targetLocation)) {
            physicsInfo.position = _targetLocation;
            physicsInfo.speed = 0;
            physicsInfo.acceleration = 0;
        } else {
            physicsInfo.direction = (_targetLocation - physicsInfo.position).normalize();
            physicsInfo.acceleration = BaseAcceleration;
        }
    }

    void Worker::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);

        if (dynamic_cast<mine_site::MineSite*>(collisionDetails.entity)) {
            timer::delay(
                CollectionTime,
                [this] { mine(); }
            );
        }

        if (dynamic_cast<base::Base*>(collisionDetails.entity)) {
            timer::delay(
                DropOffTime,
                [this] { dropOff(); }
            );
        }
    }

    void Worker::dropOff() {
        _isLoaded = false;
        renderInfo.sprite = game::Assets->workerSprite;
        _targetLocation = game::State->mineSite.getMiningLocation();
    }

    void Worker::mine() {
        _isLoaded = true;
        renderInfo.sprite = game::Assets->workerLoadedSprite;
        _targetLocation = game::State->base.getDropOffLocation();
    }
}
