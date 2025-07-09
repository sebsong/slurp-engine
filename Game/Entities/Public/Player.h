#pragma once

#include "Entity.h"

namespace game {
    class Player final : public slurp::Entity {
    public:
        Player();
        bool isParryActive;
    };
}
