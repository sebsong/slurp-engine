#pragma once
#include "RenderInfo.h"

namespace game {
    enum ZOrder {
        BACKGROUND_Z = Z_ORDER_MAX,
        BORDER_Z = Z_ORDER_MAX - 10,
        BACKGROUND_ENTITY_Z = Z_ORDER_MAX - 20,
        PROGRESS_BAR_Z = -(Z_ORDER_MAX - 30),
        UI_Z = -(Z_ORDER_MAX - 20),
        MENU_Z = -(Z_ORDER_MAX - 10),
        MOUSE_Z = -Z_ORDER_MAX,
    };
}
