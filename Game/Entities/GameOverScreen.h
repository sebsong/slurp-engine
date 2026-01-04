#pragma once
#include "Entity.h"

namespace ui {
    class GameOverScreen final : public entity::Entity {
    public:
        GameOverScreen();

    private:
        void initialize() override;
    };
}
