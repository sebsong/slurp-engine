#pragma once
#include "Entity.h"

namespace turret {
    class Turret final : public entity::Entity {
    public:
        Turret();

    private:
        bool _finishedSpawn;
        bool _flipOrbPath;
        worker::Worker* _target;
        float _currentShootCooldown;

        float _orbMaxHeight;
        float _orbMinHeight;

        void initialize() override;

        worker::Worker* findClosestCorruptedWorkerInRange(types::set_arena<Entity*> potentialTargets, float range);

        void shootAtTarget();

        void update(float dt) override;

    };
}
