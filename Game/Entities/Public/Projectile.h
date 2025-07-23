#pragma once
#include "Entity.h"

namespace game {
    class Projectile final : public slurp::Entity {
    public:
        Projectile(int index);

        void fire(const slurp::Vector2<int>& position, const slurp::Vector2<float>& direction);

    private:
        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void onParried();
        void bounce();

        bool isActive;
        bool isParried;
    };
}
