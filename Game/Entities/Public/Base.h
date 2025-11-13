#pragma once
#include "Entity.h"

// TODO: include Base.cpp in Game.cpp
// TODO: register this entity in game::initGame
// TODO: register this entity in slurp::GameState

namespace base {
    class Base final : public entity::Entity {
    public:

        Base();

        void spawnWorker() const;

        bool isSpawnLocation(const slurp::Vec2<float>& location) const;

        slurp::Vec2<float> getRandomSpawnLocation() const;

        void dropOff();

        float getProgress() const;

        bool canSpend(uint32_t amount) const;

        void spend(uint32_t amount);

    private:
        uint32_t _gold;

        void initialize() override;

        void update(float dt);

        void handleMouseAndKeyboardInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState
        ) override;
    };
}
