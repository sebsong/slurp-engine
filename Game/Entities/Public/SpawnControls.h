#pragma once
#include "Entity.h"

// TODO: include SpawnControls.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace ui {
    class SpawnControls final : public entity::Entity {
    public:
        SpawnControls(const slurp::Vec2<float>& position);

        void refresh();

    private:
        UIButton _spawnWorkerButton;
        UIButton _spawnMineSiteButton;
        UIButton _spawnTurretButton;
        timer::timer_handle _spawnWorkerTimerHandle;
        timer::timer_handle _spawnMineSiteTimerHandle;

        bool _isPlacingTurret;
        asset::Sprite _turretPlacementSprite;
        Entity _turretPlacementGuide;
        asset::Sprite _turretRangeIndicatorPlacementSprite;
        Entity _turretRangeIndicatorPlacementGuide;

        void handleMouseAndKeyboardInput(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState) override;

        void update(float dt) override;

        void startTurretPlacement();

        void stopTurretPlacement();
    };
}
