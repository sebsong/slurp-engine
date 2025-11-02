#include "Worker.h"

#include "Game.h"

namespace worker {
    static const geometry::Shape WorkerShape = {geometry::Rect, {5, 2}};
    static constexpr float BaseSpeed = 100;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static const slurp::Vec2<float> StartPos = {50, 50};
    static constexpr float CollectionTime = 2.f;
    static constexpr float DropOffTime = .5f;
    static constexpr int NumCollectionTransitions = 5;
    static const asset::Sprite* CollectionAnimationSprites[NumCollectionTransitions];

    static const float InfectionChance = 0.1f;

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
          _isLoaded(false),
          _isAtTargetLocation(false),
          _isInfected(false) {}

    Worker::Worker(const Worker& other): Entity(other) {}

    void Worker::initialize() {
        Entity::initialize();
        CollectionAnimationSprites[0] = game::Assets->workerSprite;
        CollectionAnimationSprites[1] = game::Assets->workerLoading0Sprite;
        CollectionAnimationSprites[2] = game::Assets->workerLoading1Sprite;
        CollectionAnimationSprites[3] = game::Assets->workerLoading2Sprite;
        CollectionAnimationSprites[4] = game::Assets->workerLoadedSprite;
        dropOff();
    }

    void Worker::infect() {
        // TODO: maybe this should create a new infected worker entity and destroy this one to track them separately
        _isInfected = true;
        renderInfo.sprite = game::Assets->workerInfectedSprite;
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
        //TODO: hack
        if (_isInfected) {
            renderInfo.sprite = game::Assets->workerInfectedSprite;
        }

        // debug::drawPoint(physicsInfo.position, 4, DEBUG_RED_COLOR);
        renderInfo.zOrder = physicsInfo.position.y;

        if (approxEqual(physicsInfo.position, _targetLocation)) {
            physicsInfo.position = _targetLocation;
            physicsInfo.speed = 0;
            physicsInfo.acceleration = 0;
            if (!_isAtTargetLocation) {
                if (_isLoaded) {
                    beginDropOff();
                } else {
                    beginCollect();
                }
                _isAtTargetLocation = true;
            }
        } else {
            physicsInfo.direction = (_targetLocation - physicsInfo.position).normalize();
            physicsInfo.acceleration = BaseAcceleration;
        }
    }

    void Worker::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);
    }

    void Worker::setTargetLocation(slurp::Vec2<float> newTargetLocation) {
        _targetLocation = newTargetLocation;
        _isAtTargetLocation = false;
    }

    void Worker::beginDropOff() {
        timer::delay(
            DropOffTime,
            [this] {
                audio::play(game::Assets->resourceDropOff);
                dropOff();
            }
        );
        playDropOffAnim();
    }

    static slurp::Vec2<float> getAvailableMiningLocation() {
        mine_site::MineSite* mineSite = game::State->mineSites.getRandomEnabledInstance();
        if (!mineSite) {
            return slurp::Vec2<float>::Zero;
        }
        return mineSite->getMiningLocation();
    }

    void Worker::dropOff() {
        _isLoaded = false;
        game::State->base.dropOff();
        renderInfo.sprite = game::Assets->workerSprite;
        setTargetLocation(getAvailableMiningLocation());
    }

    void Worker::playDropOffAnim() {
        float delay = DropOffTime / NumCollectionTransitions;
        for (int i = 0; i < NumCollectionTransitions; i++) {
            timer::delay(
                delay * i,
                [this, i] {
                    renderInfo.sprite = CollectionAnimationSprites[NumCollectionTransitions - 1 - i];
                }
            );
        }
    }

    void Worker::beginCollect() {
        playCollectionAnim();
        timer::delay(
            CollectionTime,
            [this] {
                collect();
                if (_isInfected) {
                    audio::play(game::Assets->errorCollect);
                } else {
                    audio::play(game::Assets->resourceCollectedLow);
                }
            }
        );
    }

    static bool rollInfection() {
        return random::randomFloat() < InfectionChance;
    }

    void Worker::collect() {
        if (!_isInfected) {
            _isLoaded = true;
        }
        renderInfo.sprite = game::Assets->workerLoadedSprite;
        if (rollInfection()) {
            infect();
        }
        if (_isInfected) {
            setTargetLocation(getAvailableMiningLocation());
        } else {
            setTargetLocation(game::State->base.getDropOffLocation());
        }
    }

    void Worker::playCollectionAnim() {
        float delay = CollectionTime / NumCollectionTransitions;
        for (int i = 0; i < NumCollectionTransitions; i++) {
            timer::delay(
                delay * i,
                [this, i] {
                    renderInfo.sprite = CollectionAnimationSprites[i];
                }
            );
        }
    }
}
