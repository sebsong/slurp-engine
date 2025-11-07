#pragma once
#include "Entity.h"

namespace turret {
    class Turret final : public entity::Entity {
    public:
        Turret();

    private:
        worker::Worker* _target;
        types::set_arena<worker::Worker*> _workersInCollider;
        float _currentShootCooldown;

        void initialize() override;

        worker::Worker* findClosestCorruptedWorkerInRange(types::set_arena<worker::Worker*> potentialTargets, float range);

        void shootAtTarget();

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override;

        void onCollisionExit(const collision::CollisionDetails& collisionDetails) override;

    };
}
