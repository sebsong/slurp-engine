#pragma once
#include "GameGlobal.h"
#include "Base.h"
#include "EntityPool.h"
#include "MineSite.h"
#include "Obstacle.h"
#include "MouseCursor.h"
#include "Worker.h"

#define MAX_NUM_WORKERS 100

namespace game {
    struct GameAssets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* borderSprite;

        asset::Sprite* baseSprite;
        asset::Sprite* mineSiteSprite;
        asset::Sprite* workerSprite;

        asset::Sprite* mouseCursorSprite;

        asset::Sound* backgroundMusic;
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

        base::Base base;
        mine_site::MineSite mineSite;
        entity::EntityPool<worker::Worker, MAX_NUM_WORKERS> workers;

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
}
