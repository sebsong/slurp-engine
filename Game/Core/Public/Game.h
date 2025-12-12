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
    struct MenuAssets {
        asset::Sprite* backgroundSprite;
    };

    struct MenuState {
        entity::Entity background;
    };

    struct Assets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* borderSprite;

        asset::Sprite* baseSprite;
        asset::SpriteAnimation* baseIdleAnim;

        asset::Sprite* mineSiteSprite;
        asset::SpriteAnimation* mineSiteSpawnAnim;

        asset::Sprite* workerSprite;
        asset::SpriteAnimation* workerLoadingAnim;
        asset::Sprite* workerLoadedSprite;
        asset::Sprite* workerCorruptedSprite;

        asset::Sprite* turretSprite;
        asset::Sprite* turretOrbSprite;
        asset::Sprite* turretPitSprite;
        asset::Sprite* turretRangeIndicatorSprite;
        asset::SpriteAnimation* turretSpawnAnim;
        asset::SpriteAnimation* turretIdleAnim;
        asset::SpriteAnimation* turretShootAnim;

        asset::Sprite* resourcesCollectedFill;

        asset::Sprite* button;
        asset::Sprite* buttonHover;
        asset::Sprite* buttonPress;
        asset::SpriteAnimation* buttonPressAnim;

        asset::Sprite* workerButtonIcon;
        asset::Sprite* mineSiteButtonIcon;
        asset::Sprite* turretButtonIcon;

        asset::Sprite* digitSprites[10];
        asset::Sprite* stopwatchPunctuationSprite;

        asset::Sprite* mouseCursorSprite;

        asset::Sprite* overlaySprite;

        asset::Sound* backgroundMusic;
        asset::Sound* resourceCollected;
        asset::Sound* resourceCollectedLow;
        asset::Sound* errorCollect;
        asset::Sound* resourceDropOff;
        asset::Sound* spawnMineSite;
        asset::Sound* turretShoot;
    };

    struct State {
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
        bool corruptionEnabled;

        entity::EntityPool<turret::Turret, MAX_NUM_TURRETS> turrets;

        ui::SpawnControls spawnControls;
        ui::ProgressBar goldProgressBar;

        ui::NumberDisplay resourcesCollectedDisplay;
        ui::StopwatchDisplay stopwatchDisplay;

        mouse_cursor::MouseCursor mouseCursor;
        entity::Entity overlay;
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
        MenuAssets menuAssets;
        MenuState menuState;
        Assets assets;
        State state;
    };

    static MenuAssets* MenuAssets;
    static MenuState* MenuState;
    static Assets* Assets;
    static State* State;

    static bool mainMenuActive = false;

    static memory::MemoryArena sceneMemory;

    void initialize(bool isInitialized);

    void handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    );

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState);

    void update(float dt);

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target);

    void corruptWorkers(int numWorkers);

    void removeCorruptibleWorker(worker::Worker* worker);
}
