#pragma once
#include "Entity.h"

// TODO: include Base.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace base {
    class Base final : public entity::Entity {
    public:
        Base();

    private:
        void initialize() override;

        void update(float dt);

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
    };
}
