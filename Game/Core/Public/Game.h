#pragma once
#include "Global.h"
#include "MouseCursor.h"
#include "Obstacle.h"
#include "Player.h"
#include "Projectile.h"

#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 10

namespace game {
    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;

        Global global;

        slurp::Entity background;
        Obstacle wallUp;
        Obstacle wallDown;
        Obstacle wallLeft;
        Obstacle wallRight;
        Obstacle obstacle1;
        Obstacle obstacle2;

        MouseCursor mouseCursor;
        Player player;
        slurp::Entity enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        Projectile projectiles[PROJECTILE_POOL_SIZE];
        slurp::Entity colorPaletteSwatch[COLOR_PALETTE_SIZE];
    };

    static const platform::PlatformDll* GlobalPlatformDll;
    static GameState* GlobalGameState;
    static render::ColorPalette GlobalColorPalette;

    void init(GameState& gameState, slurp::EntityManager& entityManager);

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
