#pragma once
#include "GameGlobal.h"
#include "MouseCursor.h"
#include "Obstacle.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Worker.h"

#define NUM_ENEMIES 4
#define PROJECTILE_POOL_SIZE 10

namespace game {
    struct GameAssets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* borderSprite;

        asset::Sprite* baseSprite;

        asset::Sprite* goldSprite;

        asset::Sprite* workerSprite;

        asset::Sprite* enemySprite;
        asset::Sprite* mouseCursorSprite;
        asset::Sprite* playerSprite;
        asset::Sprite* playerParrySprite;
        asset::Sprite* projectileSprite;
        asset::Sprite* projectileParriedSprite;

        asset::Sound* backgroundMusic;
        asset::Sound* projectileHitSound;
    };

    struct GameState {
        uint32_t randomSeed;
        bool isInitialized;

        global::GameGlobal global;

        entity::Entity background;
        entity::Entity border;
        obstacle::Obstacle wallUp;
        obstacle::Obstacle wallDown;
        obstacle::Obstacle wallLeft;
        obstacle::Obstacle wallRight;

        entity::Entity base;
        entity::Entity gold;
        worker::Worker worker;

        mouse_cursor::MouseCursor mouseCursor;
        player::Player player;
        enemy::Enemy enemies[NUM_ENEMIES];
        uint32_t projectileIdx;
        projectile::Projectile projectiles[PROJECTILE_POOL_SIZE];
    };

    struct GameSystems {
        GameAssets assets;
        GameState state;
    };

    static GameAssets* Assets;
    static GameState* State;

    void initGame(bool isInitialized);
}
