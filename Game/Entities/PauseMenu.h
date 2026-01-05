#pragma once
#include "Entity.h"
#include "Button.h"

namespace ui {
    class PauseMenu final : public entity::Entity {
    public:
        PauseMenu();

        void initialize() override;

        void toggle();

    private:
        Button _resumeButton;
        Button _mainMenuButton;
        Button _exitButton;
    };
}
