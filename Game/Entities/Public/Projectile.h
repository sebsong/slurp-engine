#pragma once
#include "Entity.h"

namespace game {
    class Projectile final : public slurp::Entity {
    public:
        Projectile(int index);

        void fire(const slurp::Vector2<int>& position);

    private:
        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void onParried();

        void bounce();

        bool _isActive;
        bool _isParried;
        const Entity* _target = nullptr;
    };
}
