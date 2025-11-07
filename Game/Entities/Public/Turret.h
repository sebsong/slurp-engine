#pragma once
#include "Entity.h"

namespace turret {
    class Turret final : public entity::Entity {
    public:
        Turret();

    private:
        void initialize() override;

        void update(float dt) override;

    };
}
