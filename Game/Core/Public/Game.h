#pragma once
#include "Base.h"
#include "EntityPool.h"
#include "MineSite.h"
#include "MineSiteSpawner.h"
#include "Obstacle.h"
#include "MouseCursor.h"
#include "Worker.h"
#include "Turret.h"
#include "UIButton.h"
#include "NumberDisplay.h"
#include "ProgressBar.h"
#include "SpawnControls.h"
#include "StopwatchDisplay.h"

#define MAX_NUM_WORKERS 10000
#define MAX_NUM_TURRETS 100
#define MAX_NUM_MINE_SITES 100

namespace ui_button {
    class UIButton;
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

        asset::Sprite* turretSprite;
        asset::Sprite* turretRangeIndicatorSprite;
        asset::SpriteAnimation* turretShootAnimation;

        asset::Sprite* resourcesCollectedFill;

        asset::Sprite* workerButton;
        asset::Sprite* workerButtonHover;
        asset::Sprite* workerButtonPress;
        asset::SpriteAnimation* workerButtonPressAnim;

        asset::Sprite* mineSiteButton;
        asset::Sprite* mineSiteButtonHover;
        asset::Sprite* mineSiteButtonPress;
        asset::SpriteAnimation* mineSiteButtonPressAnim;

        asset::Sprite* turretButton;
        asset::Sprite* turretButtonHover;
        asset::Sprite* turretButtonPress;

        asset::Sprite* digitSprites[10];
        asset::Sprite* stopwatchPunctuationSprite;

        asset::Sprite* mouseCursorSprite;

        asset::Sound* backgroundMusic;
        asset::Sound* resourceCollected;
        asset::Sound* resourceCollectedLow;
        asset::Sound* errorCollect;
        asset::Sound* resourceDropOff;
        asset::Sound* spawnMineSite;
        asset::Sound* turretShoot;
    };

    struct GameState {
        uint32_t randomSeed;
        audio::sound_id bgmId;

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
        types::vector_arena<worker::Worker*> corruptibleWorkers;

        entity::EntityPool<turret::Turret, MAX_NUM_TURRETS> turrets;
        entity::EntityPool<entity::Entity, MAX_NUM_TURRETS> turretsRangeIndicators;

        ui::SpawnControls spawnControls;
        ui::ProgressBar goldProgressBar;

        ui::NumberDisplay resourcesCollectedDisplay;
        ui::StopwatchDisplay stopwatchDisplay;

        mouse_cursor::MouseCursor mouseCursor;
    };

    enum ZOrder {
        BACKGROUND_Z = Z_ORDER_MAX,
        BORDER_Z = Z_ORDER_MAX - 1,
        BACKGROUND_ENTITY_Z = Z_ORDER_MAX - 2,
        PROGRESS_BAR_Z = -(Z_ORDER_MAX - 2),
        UI_Z = -(Z_ORDER_MAX - 1),
        MOUSE_Z = -Z_ORDER_MAX,
    };

    struct GameSystems {
        GameAssets assets;
        GameState state;
    };

    static GameAssets* Assets;
    static GameState* State;

    void initialize(bool isInitialized);

    void handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    );

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState);

    void update(float dt);

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target);
}
