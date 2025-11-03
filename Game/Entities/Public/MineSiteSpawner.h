#pragma once
#include "Entity.h"

namespace mine_site {
    class MineSiteSpawner final : public entity::Entity {
    public:
        MineSiteSpawner();

        void spawnMineSite();

    private:
        void initialize() override;

        void update(float dt) override;
    };
}
