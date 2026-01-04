#pragma once
#include "Entity.h"
#include "Timer.h"
#include "Button.h"

// TODO: include SpawnControls.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace ui {
    class SpawnControls final : public entity::Entity {
    public:
        SpawnControls() = default;

        SpawnControls(const slurp::Vec2<float>& position);

        void initialize() override;

        void handleMouseAndKeyboardInput(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState) override;

        void update(float dt) override;

        void refresh();

    private:
        Button _spawnWorkerButton;
        Button _spawnMineSiteButton;
        Button _spawnTurretButton;
        timer::timer_handle _spawnWorkerTimerHandle;
        timer::timer_handle _spawnMineSiteTimerHandle;

        bool _isPlacingTurret;
        Entity _turretPlacementGuide;

        void startTurretPlacement();

        void stopTurretPlacement();
    };
}
