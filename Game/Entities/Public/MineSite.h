#pragma once
#include "Entity.h"

// TODO: include MineSite.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace mine_site {
    class MineSite final : public entity::Entity {
    public:
        MineSite();

        slurp::Vec2<float> getMiningLocation() const;

    private:
        void update(float dt) override;
    };
}
