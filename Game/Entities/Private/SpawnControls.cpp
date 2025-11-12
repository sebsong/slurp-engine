#include "SpawnControls.h"

namespace ui {
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
                      _spawnWorkerButton.renderInfo.animation = *game::Assets->workerButtonPressAnim;
                      _spawnWorkerButton.renderInfo.animation.play(true, WorkerBuildTime);
                      timer::start(
                          _spawnWorkerTimerHandle,
                          WorkerBuildTime,
                          true,
                          [] { game::State->base.spawnWorker(); }
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
                  [] {},
                  [] {}
              )
          ) {}

    void SpawnControls::update(float dt) {
        Entity::update(dt);
    }
}
