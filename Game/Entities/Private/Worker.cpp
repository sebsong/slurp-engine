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

    static const float CorruptionChance = 0.1f;

    static const uint32_t MaxNumAntibodies = 3;
    static const float PurifyDeceleration = -10;
    static const float PurifyDelay = 3;

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
          _isCorrupted(false),
          _attachedAntibodies(types::set_arena<antibody::Antibody*>()) {}

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

    void Worker::corrupt() {
        game::State->targetableCorruptedWorkers.push_back(this);
        _isCorrupted = true;
        renderInfo.sprite = game::Assets->workerCorruptedSprite;
        for (auto& antibody: game::State->antibodies) {
            if (!antibody->hasTarget()) {
                antibody->findTarget();
            }
        }
    }


    bool Worker::isCorrupted() const {
        return _isCorrupted;
    }

    void Worker::purify() {
        _isCorrupted = false;
        for (auto antibody: _attachedAntibodies) {
            game::State->antibodies.recycleInstance(antibody);
        }
        _attachedAntibodies.clear();
        physicsInfo.maxSpeed = BaseSpeed;
        renderInfo.sprite = game::Assets->workerSprite;
    }

    void Worker::registerAntibody(antibody::Antibody* antibody) {
        _attachedAntibodies.insert(antibody);

        if (_attachedAntibodies.size() >= MaxNumAntibodies) {
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
    }

    void Worker::applyAntibodyEffects(float maxSpeedMultiplier) {
        physicsInfo.maxSpeed *= maxSpeedMultiplier;
        if (_attachedAntibodies.size() >= MaxNumAntibodies) {
            _isPurifying = true;
            physicsInfo.acceleration = PurifyDeceleration;
            timer::delay(
                PurifyDelay,
                [this] {
                    purify();
                    _isPurifying = false;
                }
            );
        }
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

        // debug::drawPoint(physicsInfo.position, 4, DEBUG_RED_COLOR);
        renderInfo.zOrder = physicsInfo.position.y;

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
            setTargetLocation(getAvailableMiningLocation());
        } else {
            _isLoaded = true;
            setTargetLocation(game::State->base.getDropOffLocation());
            renderInfo.sprite = game::Assets->workerLoadedSprite;
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
