#include "Worker.h"

#include "Game.h"

namespace worker {
    static const geometry::Shape WorkerShape = {geometry::Rect, {5, 3}};
    static constexpr float BaseSpeed = 100;
    static constexpr float BaseAcceleration = BaseSpeed * 16;
    static const slurp::Vec2<float> StartPos = {50, 50};
    static constexpr float CollectionTime = 2.f;
    static constexpr float DropOffTime = .5f;
    static constexpr int NumCollectionTransitions = 5;
    static const float MaxMineSpotWaitTime = .5f;

    static const float CorruptionChance = .05f;
    static const uint8_t StartingCorruption = 3;

    static const float PurifyDeceleration = -10;
    static const float PurifyDelay = 3;


    Worker::Worker()
        : Entity(
              "Worker",
              render::RenderInfo(
                  slurp::Globals->GameAssets->workerSprite,
                  true,
                  true,
                  {0, 3}
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
          _isCorrupted(false),
          _corruptionRemaining(StartingCorruption) {}

    void Worker::initialize() {
        Entity::initialize();
        findNewMiningLocation();
    }

    void Worker::corrupt() {
        game::State->targetableCorruptedWorkers.push_back(this);
        _isCorrupted = true;
        _corruptionRemaining = StartingCorruption;
        renderInfo.sprite = game::Assets->workerCorruptedSprite;
    }

    bool Worker::isCorrupted() const {
        return _isCorrupted;
    }

    void Worker::purify() {
        _corruptionRemaining--;
        if (_corruptionRemaining <= 0) {
            _isCorrupted = false;
            for (
                auto it = game::State->targetableCorruptedWorkers.begin();
                it != game::State->targetableCorruptedWorkers.end();
                it++
            ) {
                if (*it == this) {
                    game::State->targetableCorruptedWorkers.erase(it);
                    break;
                }
            }
        }
        physicsInfo.maxSpeed = BaseSpeed;
        renderInfo.sprite = game::Assets->workerSprite;
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

    void Worker::update(float dt) {
        Entity::update(dt);
        //TODO: hack
        if (_isCorrupted) {
            renderInfo.sprite = game::Assets->workerCorruptedSprite;
        }

        if (_isIdle) {
            physicsInfo.speed = 0;
            physicsInfo.acceleration = 0;
            findNewMiningLocation();
            return;
        }

        if (game::almostAtTarget(this, _targetLocation)) {
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
            if (!_isPurifying) {
                physicsInfo.acceleration = BaseAcceleration;
            }
        }
    }

    void Worker::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {
        Entity::onCollisionEnter(collisionDetails);
    }

    void Worker::setTargetLocation(slurp::Vec2<float> newTargetLocation) {
        _targetLocation = newTargetLocation;
        _isAtTargetLocation = false;
        _isIdle = false;

        if (_targetLocation.isZero()) {
            idle();
            return;
        }

        if (_targetLocation != game::State->base.getDropOffLocation()) {
            occupyMiningLocation(_targetLocation);
        }
    }

    void Worker::occupyMiningLocation(slurp::Vec2<float> location) {
        for (auto it = game::State->mineSpots.begin(); it != game::State->mineSpots.end(); it++) {
            if (*it == location) {
                game::State->mineSpots.erase(it);
                break;
            }
        }
    }

    void Worker::leaveMiningLocation() {
        if (!_targetLocation.isZero() && _targetLocation != game::State->base.getDropOffLocation()) {
            game::State->mineSpots.push_back(_targetLocation);
            _targetLocation = slurp::Vec2<float>::Zero;
        }
    }

    static slurp::Vec2<float> getAvailableMiningLocation() {
        return random::pickRandom(game::State->mineSpots, slurp::Vec2<float>::Zero);
    }


    void Worker::findNewMiningLocation() {
        leaveMiningLocation();
        setTargetLocation(getAvailableMiningLocation());
    }

    void Worker::beginDropOff() {
        playAnimation(game::Assets->workerLoadingAnim, DropOffTime, false, true);
        timer::delay(
            DropOffTime,
            [this] {
                audio::play(game::Assets->resourceDropOff);
                dropOff();
            }
        );
    }

    void Worker::dropOff() {
        _isLoaded = false;
        game::State->base.dropOff();
        renderInfo.sprite = game::Assets->workerSprite;
        findNewMiningLocation();
    }

    void Worker::beginCollect() {
        if (!_isCorrupted) {
            playAnimation(game::Assets->workerLoadingAnim, CollectionTime);
        }
        timer::delay(
            CollectionTime,
            [this] {
                collect();
                if (_isCorrupted) {
                    audio::play(game::Assets->errorCollect);
                } else {
                    audio::play(game::Assets->resourceCollectedLow);
                }
            }
        );
    }

    static bool rollCorruption() {
        return random::randomFloat() < CorruptionChance;
    }

    void Worker::collect() {
        if (rollCorruption()) {
            corrupt();
        }
        if (_isCorrupted) {
            findNewMiningLocation();
        } else {
            _isLoaded = true;
            leaveMiningLocation();
            setTargetLocation(game::State->base.getDropOffLocation());
            renderInfo.sprite = game::Assets->workerLoadedSprite;
        }
    }

    void Worker::idle() {
        _isIdle = true;
    }
}
