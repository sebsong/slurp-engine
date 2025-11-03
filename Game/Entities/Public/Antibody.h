#pragma once
#include "Entity.h"

namespace antibody {
    class Antibody final : public entity::Entity {
    public:
        Antibody();

    private:
        Entity* _target;
        bool  _isAtTarget;

        void initialize() override;

        void update(float dt) override;

        void onCollisionEnter(const collision::CollisionDetails& collisionDetails) override {};

        void findTarget();
    };
}
