#pragma once
#include "Entity.h"

namespace ui {
    class PauseMenu final : public entity::Entity {
    public:
        PauseMenu();

    private:
        Entity _menu;
        Entity _screenCover;
    };
}
