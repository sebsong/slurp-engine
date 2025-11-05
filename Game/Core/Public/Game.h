#pragma once
#include "GameGlobal.h"
#include "Base.h"
#include "EntityPool.h"
#include "MineSite.h"
#include "MineSiteSpawner.h"
#include "Obstacle.h"
#include "MouseCursor.h"
#include "Worker.h"
#include "Antibody.h"

#define MAX_NUM_WORKERS 100000
#define MAX_NUM_ANTIBODIES 100000
#define MAX_NUM_MINE_SITES 100

namespace antibody {
    class Antibody;
}

namespace game {
    struct GameAssets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* borderSprite;

        asset::Sprite* baseSprite;
        asset::Sprite* mineSiteSprite;
        asset::SpriteAnimation* mineSiteSpawnAnim;

        asset::Sprite* workerSprite;
        asset::SpriteAnimation* workerLoadingAnim;
        asset::Sprite* workerLoadedSprite;
        asset::Sprite* workerCorruptedSprite;
        asset::Sprite* antibodySprite;

        asset::Sprite* storageSilo;
        asset::Sprite* storageSiloFill;

        asset::Sprite* mouseCursorSprite;

        asset::Sound* backgroundMusic;
        asset::Sound* resourceCollected;
        asset::Sound* resourceCollectedLow;
        asset::Sound* errorCollect;
        asset::Sound* resourceDropOff;
        asset::Sound* spawnMineSite;
    };

    struct GameState {
        uint32_t randomSeed;

        global::GameGlobal global;

        entity::Entity background;
        entity::Entity border;
        obstacle::Obstacle wallUp;
        obstacle::Obstacle wallDown;
        obstacle::Obstacle wallLeft;
        obstacle::Obstacle wallRight;

        base::Base base;
        mine_site::MineSiteSpawner mineSiteSpawner;
        entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES> mineSites;
        types::deque_arena<slurp::Vec2<float>> mineSpots;

        entity::EntityPool<worker::Worker, MAX_NUM_WORKERS> workers;
        types::deque_arena<worker::Worker*> targetableCorruptedWorkers;

        entity::EntityPool<antibody::Antibody, MAX_NUM_WORKERS> antibodies;

        entity::Entity storageSilo;
        entity::Entity storageSiloFill;

        mouse_cursor::MouseCursor mouseCursor;
    };

    enum ZOrder {
        BACKGROUND_Z = Z_ORDER_MAX,
        BORDER_Z = Z_ORDER_MAX - 1,
        MOUSE_Z = -Z_ORDER_MAX,
    };

    struct GameSystems {
        GameAssets assets;
        GameState state;
    };

    static GameAssets* Assets;
    static GameState* State;

    void initGame(bool isInitialized);

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target);
}
