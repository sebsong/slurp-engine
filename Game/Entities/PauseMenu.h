#pragma once
#include "Entity.h"

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
