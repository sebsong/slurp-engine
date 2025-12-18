#pragma once
#include "Entity.h"
#include "UIButton.h"

namespace ui {
    class PauseMenu final : public entity::Entity {
    public:
        PauseMenu();

        void toggle();

    private:
        UIButton _resumeButton;
        UIButton _exitButton;
    };
}
