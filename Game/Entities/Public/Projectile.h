#pragma once
#include "Entity.h"

namespace game {
    class Projectile final : public slurp::Entity {
    public:
        Projectile(int index);

    private:
        void onCollisionEnter(const Entity* other) override;
    };
}
