#include "SpawnControls.h"

#include "Game.h"
#include "Matrix.h"
#include "Settings.h"

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

    static const geometry::Shape buttonShape = {geometry::Rect, {25, 16}};

    SpawnControls::SpawnControls(const slurp::Vec2<float>& position)
        : Entity("SpawnControls"),
          _spawnWorkerButton(
              Button(
                  game::Assets->workerSprite,
                  game::Assets->buttonSprite,
                  game::Assets->buttonHoverSprite,
                  game::Assets->buttonPressSprite,
                  buttonShape,
                  {position.x - 30, position.y},
                  slurp::KeyboardCode::NUM_1,
                  [this](Button* button) {
                      button->playAnimation(game::Assets->buttonPressAnim, WorkerBuildTime, true);
                      timer::start(
                          _spawnWorkerTimerHandle,
                          WorkerBuildTime,
                          true,
                          [] {
                              game::GameScene->base.spend(WorkerBuildCost);
                              game::GameScene->base.spawnWorker();
                          }
                      );
                  },
                  [](Button* _) {},
                  [this](Button* button) {
                      button->stopAnimation();
                      timer::cancel(_spawnWorkerTimerHandle);
                  },
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  }
              )
          ),
          _spawnMineSiteButton(
              Button(
                  game::Assets->mineSiteButtonIcon,
                  game::Assets->buttonSprite,
                  game::Assets->buttonHoverSprite,
                  game::Assets->buttonPressSprite,
                  buttonShape,
                  {position.x, position.y},
                  slurp::KeyboardCode::NUM_2,
                  [this](Button* button) {
                      button->playAnimation(game::Assets->buttonPressAnim, MineSiteBuildTime, true);
                      timer::start(
                          _spawnMineSiteTimerHandle,
                          MineSiteBuildTime,
                          true,
                          [] {
                              game::GameScene->base.spend(MineSiteBuildCost);
                              game::GameScene->mineSiteSpawner.spawnMineSite();
                          }
                      );
                  },
                  [](Button* _) {},
                  [this](Button* button) {
                      button->stopAnimation();
                      timer::cancel(_spawnMineSiteTimerHandle);
                  },
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  }
              )
          ),
          _spawnTurretButton(
              Button(
                  game::Assets->turretButtonIcon,
                  game::Assets->buttonSprite,
                  game::Assets->buttonHoverSprite,
                  game::Assets->buttonPressSprite,
                  buttonShape,
                  {position.x + 30, position.y},
                  slurp::KeyboardCode::NUM_3,
                  [this](Button* _) {
                      if (!_isPlacingTurret) {
                          startTurretPlacement();
                      } else {
                          stopTurretPlacement();
                      }
                  },
                  [](Button* _) {},
                  [](Button* _) {},
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  }
              )
          ),
          _spawnWorkerTimerHandle(-1),
          _spawnMineSiteTimerHandle(-1),
          _isPlacingTurret(false),
          _turretPlacementGuide(
              Entity(
                  "Turret Placement Guide",
                  render::RenderInfo(
                      (render::SpriteInstance[2]){
                          render::SpriteInstance(
                              game::Assets->turretSprite,
                              turret::RenderOffset
                          ),

                          render::SpriteInstance(
                              game::Assets->turretRangeIndicatorSprite,
                              game::BACKGROUND_ENTITY_Z,
                              turret::RenderOffset
                          )
                      }
                  ),
                  physics::PhysicsInfo(),
                  collision::CollisionInfo()
              )
          ) {}

    void SpawnControls::initialize() {
        Entity::initialize();

        _spawnWorkerTimerHandle = timer::reserveHandle();
        _spawnMineSiteTimerHandle = timer::reserveHandle();

        _turretPlacementGuide.enabled = false;
        _turretPlacementGuide.applyAlpha(0.5f);

        scene::registerEntity(scene, &_spawnWorkerButton);
        scene::registerEntity(scene, &_spawnMineSiteButton);
        scene::registerEntity(scene, &_spawnTurretButton);
        scene::registerEntity(scene, &_turretPlacementGuide);
    }

    void SpawnControls::refresh() {
        game::GameScene->base.canSpend(WorkerBuildCost)
            ? _spawnWorkerButton.enableButton()
            : _spawnWorkerButton.disableButton();
        game::GameScene->base.canSpend(MineSiteBuildCost)
            ? _spawnMineSiteButton.enableButton()
            : _spawnMineSiteButton.disableButton();
        if (game::GameScene->base.canSpend(TurretBuildCost)) {
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
            if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
                stopTurretPlacement();
                game::GameScene->base.spend(TurretBuildCost);
                turret::Turret* turret = game::GameScene->turrets.nextInstance();
                turret->physicsInfo.position = _turretPlacementGuide.physicsInfo.position;
                game::GameScene->turrets.enableInstance(turret);
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
    }
}
