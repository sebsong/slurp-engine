#pragma once
#include "Global.h"
#include "MouseCursor.h"
#include "Obstacle.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Sprite.h"

#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 10

namespace game {
    struct GameAssets {
        render::ColorPalette colorPalette;

        asset::Sprite borderSprite;
        asset::Sprite enemySprite;
        asset::Sprite mouseCursorSprite;
        asset::Sprite playerSprite;
        asset::Sprite playerParrySprite;
        asset::Sprite projectileSprite;
        asset::Sprite projectileParriedSprite;

        asset::PlayingSound* backgroundMusic;
        asset::PlayingSound* projectileHitSound;
    };

    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;

        global::Global global;

        // slurp::Entity background;
        slurp::Entity border;
        obstacle::Obstacle wallUp;
        obstacle::Obstacle wallDown;
        obstacle::Obstacle wallLeft;
        obstacle::Obstacle wallRight;
        obstacle::Obstacle obstacle1;
        obstacle::Obstacle obstacle2;

        slurp::Entity triangleBackground;
        slurp::Entity triangle;

        slurp::Entity testAlpha;

        mouse_cursor::MouseCursor mouseCursor;
        player::Player player;
        enemy::Enemy enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        projectile::Projectile projectiles[PROJECTILE_POOL_SIZE];
        slurp::Entity colorPaletteSwatch[COLOR_PALETTE_SIZE];
    };

    static GameAssets* GlobalGameAssets;
    static GameState* GlobalGameState;

    void initGame(
        GameAssets& gameAssets,
        GameState& gameState,
        slurp::EntityManager& entityManager
    );

    render::Pixel getColor(render::ColorPaletteIdx colorPaletteIdx);
}
