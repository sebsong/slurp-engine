#pragma once
#include "Entity.h"

namespace mine_site {
    class MineSiteSpawner final : public entity::Entity {
    public:
        MineSiteSpawner();

    private:
        void initialize() override;

        void update(float dt) override;
    };
}
