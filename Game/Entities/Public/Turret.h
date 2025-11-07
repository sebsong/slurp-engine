#pragma once
#include "Entity.h"

namespace turret {
    class Turret final : public entity::Entity {
    public:
        Turret();

    private:
        worker::Worker* _target;
        float _currentShootCooldown;

        void initialize() override;

        worker::Worker* findCorruptedWorkerInRange(types::deque_arena<worker::Worker*> potentialTargets, float range);

        void update(float dt) override;

    };
}
