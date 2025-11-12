#include "SpawnControls.h"

namespace ui {
    static constexpr uint32_t WorkerBuildCost = 5;
    static constexpr uint32_t MineSiteBuildCost = 20;
    static constexpr uint32_t TurretBuildCost = 20;

    static constexpr float WorkerBuildTime = 1.f;
    static constexpr float MineSiteBuildTime = 2.f;

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
                  [] {
                      game::State->base.spend(MineSiteBuildCost);
                      game::State->mineSiteSpawner.spawnMineSite();
                  },
                  [] {}
              )
          ),
          _spawnTurretButton(
              UIButton(
                  game::Assets->turretButton,
                  game::Assets->turretButtonHover,
                  game::Assets->turretButtonPress,
                  {position.x + 30, position.y},
                  slurp::KeyboardCode::NUM_3,
                  [] {
                      game::State->base.spend(TurretBuildCost);
                  },
                  [] {}
              )
          ) {}

    void SpawnControls::refresh() {
        game::State->base.canSpend(WorkerBuildCost)
            ? _spawnWorkerButton.enableButton()
            : _spawnWorkerButton.disableButton();
        game::State->base.canSpend(MineSiteBuildCost)
            ? _spawnMineSiteButton.enableButton()
            : _spawnMineSiteButton.disableButton();
        game::State->base.canSpend(TurretBuildCost)
            ? _spawnTurretButton.enableButton()
            : _spawnTurretButton.disableButton();
    }

    void SpawnControls::update(float dt) {
        Entity::update(dt);
    }
}
