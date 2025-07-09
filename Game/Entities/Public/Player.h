#pragma once

#include "Entity.h"

namespace game {
    class Player final : public slurp::Entity {
    public:
        Player();
        bool isParryActive;
    private:
       void onCollisionEnter(const Entity* otherEntity);
       void onCollisionExit(const Entity* other);
    };
}
