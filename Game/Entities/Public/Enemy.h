#pragma once
#include "Entity.h"

namespace enemy {
    class Enemy final : public slurp::Entity {
    public:
        Enemy(int i);

    private:
        void initialize() override;
    };
}
