#pragma once
#include "Entity.h"
#include "Timer.h"

namespace projectile {
    class Projectile final : public slurp::Entity {
    public:
        Projectile(int index);

        void fire(const slurp::Vec2<float>& position);

    private:
        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void onParried();

        void bounce();

        bool _isActive;
        bool _isParried;
        const Entity* _target;
        const timer::timer_handle _parriedTimerHandle;
    };
}
