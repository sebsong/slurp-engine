#include "SpawnControls.h"

#include "Game.h"

namespace ui {
    static constexpr uint32_t WorkerBuildCost = 5;
    static constexpr uint32_t MineSiteBuildCost = 20;
    static constexpr uint32_t TurretBuildCost = 0;

    static constexpr float WorkerBuildTime = 1.f;
    static constexpr float MineSiteBuildTime = 2.f;

    static const slurp::Mat22<float> TurretPlacementBounds = {
        {-WORLD_WIDTH_MAX + 20, -WORLD_HEIGHT_MAX + 15},
        {WORLD_WIDTH_MAX - 20, WORLD_HEIGHT_MAX - 55}
    };

    SpawnControls::SpawnControls(const slurp::Vec2<float>& position)
        : Entity("SpawnControls"),
          _spawnWorkerButton(
              UIButton(
                  game::Assets->workerButton,
                  game::Assets->workerButtonHover,
                  game::Assets->workerButtonPress,
                  {position.x - 30, position.y},
                  slurp::KeyboardCode::NUM_1,
                  [this] {
                      _spawnWorkerButton.playAnimation(game::Assets->workerButtonPressAnim, WorkerBuildTime, true);
                      timer::start(
                          _spawnWorkerTimerHandle,
                          WorkerBuildTime,
                          true,
                          [] {
                              game::State->base.spend(WorkerBuildCost);
                              game::State->base.spawnWorker();
                          }
                      );
                  },
                  [this] {
                      _spawnWorkerButton.renderInfo.animation.stop();
                      timer::cancel(_spawnWorkerTimerHandle);
                  }
              )
          ),
          _spawnMineSiteButton(
              UIButton(
                  game::Assets->mineSiteButton,
                  game::Assets->mineSiteButtonHover,
                  game::Assets->mineSiteButtonPress,
                  {position.x, position.y},
                  slurp::KeyboardCode::NUM_2,
                  [this] {
                      _spawnMineSiteButton.playAnimation(
                          game::Assets->mineSiteButtonPressAnim,
                          MineSiteBuildTime,
                          true
                      );
                      timer::start(
                          _spawnMineSiteTimerHandle,
                          MineSiteBuildTime,
                          true,
                          [] {
                              game::State->base.spend(MineSiteBuildCost);
                              game::State->mineSiteSpawner.spawnMineSite();
                          }
                      );
                  },
                  [this] {
                      _spawnMineSiteButton.renderInfo.animation.stop();
                      timer::cancel(_spawnMineSiteTimerHandle);
                  }
              )
          ),
          _spawnTurretButton(
              UIButton(
                  game::Assets->turretButton,
                  game::Assets->turretButtonHover,
                  game::Assets->turretButtonPress,
                  {position.x + 30, position.y},
                  slurp::KeyboardCode::NUM_3,
                  [this] {
                      if (!_isPlacingTurret) {
                          startTurretPlacement();
                      } else {
                          stopTurretPlacement();
                      }
                  },
                  [] {}
              )
          ),
          _turretPlacementSprite(*game::Assets->turretSprite),
          _turretPlacementGuide(
              Entity(
                  "Turret Placement Guide",
                  render::RenderInfo(
                      &_turretPlacementSprite,
                      true,
                      true,
                      turret::RenderOffset
                  ),
                  physics::PhysicsInfo(),
                  collision::CollisionInfo()
              )
          ),
          _turretRangeIndicatorPlacementSprite(*game::Assets->turretRangeIndicatorSprite),
          _turretRangeIndicatorPlacementGuide(
              Entity(
                  "Turret Range Indicator Guide",
                  render::RenderInfo(
                      &_turretRangeIndicatorPlacementSprite,
                      true,
                      game::BACKGROUND_ENTITY_Z,
                      turret::RenderOffset
                  ),
                  physics::PhysicsInfo(),
                  collision::CollisionInfo()
              )
          ) {
        _spawnWorkerTimerHandle = timer::reserveHandle();
        _spawnMineSiteTimerHandle = timer::reserveHandle();

        _turretPlacementGuide.enabled = false;
        _turretPlacementSprite.material.alpha *= 0.5f;
        _turretRangeIndicatorPlacementGuide.enabled = false;
        _turretRangeIndicatorPlacementSprite.material.alpha *= 0.5f;
    }

    void SpawnControls::refresh() {
        game::State->base.canSpend(WorkerBuildCost)
            ? _spawnWorkerButton.enableButton()
            : _spawnWorkerButton.disableButton();
        game::State->base.canSpend(MineSiteBuildCost)
            ? _spawnMineSiteButton.enableButton()
            : _spawnMineSiteButton.disableButton();
        if (game::State->base.canSpend(TurretBuildCost)) {
            _spawnTurretButton.enableButton();
        } else {
            stopTurretPlacement();
            _spawnTurretButton.disableButton();
        }
    }

    void SpawnControls::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        if (mouseState.justPressed(slurp::MouseCode::RightClick)) {
            stopTurretPlacement();
        }

        if (_isPlacingTurret) {
            slurp::Vec2<float> turretPlacementPosition = math::getClamped(
                mouseState.position,
                TurretPlacementBounds.i,
                TurretPlacementBounds.j
            );
            _turretPlacementGuide.physicsInfo.position = turretPlacementPosition;
            _turretPlacementGuide.enable();
            _turretRangeIndicatorPlacementGuide.physicsInfo.position = turretPlacementPosition;
            _turretRangeIndicatorPlacementGuide.enable();
            if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
                stopTurretPlacement();
                game::State->base.spend(TurretBuildCost);
                turret::Turret* turret = game::State->turrets.nextInstance();
                turret->physicsInfo.position = _turretPlacementGuide.physicsInfo.position;
                game::State->turrets.enableInstance(turret);
            }
        }
    }

    void SpawnControls::update(float dt) {
        Entity::update(dt);
    }

    void SpawnControls::startTurretPlacement() {
        _isPlacingTurret = true;
    }

    void SpawnControls::stopTurretPlacement() {
        _isPlacingTurret = false;
        _turretPlacementGuide.enabled = false;
        _turretRangeIndicatorPlacementGuide.enabled = false;
    }
}
