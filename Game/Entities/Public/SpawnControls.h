#pragma once
#include "Entity.h"

// TODO: include SpawnControls.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace ui {
    class SpawnControls final : public entity::Entity {
    public:
        SpawnControls(const slurp::Vec2<float>& position);

    private:
        UIButton _spawnWorkerButton;
        UIButton _spawnMineSiteButton;
        UIButton _spawnTurretButton;
        timer::timer_handle _spawnWorkerTimerHandle;
        timer::timer_handle _spawnMineSiteTimerHandle;

        void update(float dt) override;
    };
}
