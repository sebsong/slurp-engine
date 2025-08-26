#pragma once
#include "Global.h"
#include "MouseCursor.h"
#include "Obstacle.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"

#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 10

namespace game {
    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;

        global::Global global;

        slurp::Entity background;
        obstacle::Obstacle wallUp;
        obstacle::Obstacle wallDown;
        obstacle::Obstacle wallLeft;
        obstacle::Obstacle wallRight;
        obstacle::Obstacle obstacle1;
        obstacle::Obstacle obstacle2;

        slurp::Entity triangle;

        slurp::Entity testAlpha;

        mouse_cursor::MouseCursor mouseCursor;
        player::Player player;
        enemy::Enemy enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        projectile::Projectile projectiles[PROJECTILE_POOL_SIZE];
        slurp::Entity colorPaletteSwatch[COLOR_PALETTE_SIZE];
    };

    static const platform::PlatformDll* GlobalPlatformDll;
    static const render::RenderApi* GlobalRenderApi;
    static GameState* GlobalGameState;
    static audio::SoundManager* GlobalSoundManager;
    static render::ColorPalette GlobalColorPalette;

    void initGame(
        GameState& gameState,
        slurp::EntityManager& entityManager,
        audio::SoundManager& soundManager,
        const platform::PlatformDll& platformDll,
        const render::RenderApi& renderApi
    );

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
