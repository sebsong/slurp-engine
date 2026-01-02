#pragma once
#include "AudioPlayer.h"
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
#include "PauseMenu.h"
#include "ProgressBar.h"
#include "Scene.h"
#include "SpawnControls.h"
#include "StopwatchDisplay.h"

#define MAX_NUM_WORKERS 10000
#define MAX_NUM_TURRETS 1000
#define MAX_NUM_MINE_SITES 1000

namespace game {
    struct MainMenuAssets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* titleTextSprite;
        asset::Sprite* slurpEngineTextSprite;
        asset::Sprite* playButtonTextSprite;
        asset::Sprite* exitButtonTextSprite;
        asset::Sprite* buttonSprite;
        asset::Sprite* buttonHoverSprite;
        asset::Sprite* buttonPressSprite;
        asset::Sprite* mouseCursorSprite;

        asset::Sound* bgmIntro;
        asset::Sound* bgmMain;
        asset::Sound* buttonHover;
    };

    struct MainMenuState : scene::Scene {
        audio::PlayingSound* bgm;

        entity::Entity background;
        entity::Entity titleText;
        entity::Entity slurpEngineText;
        ui::UIButton playButton;
        ui::UIButton exitButton;

        mouse_cursor::MouseCursor mouseCursor;

        void load() override;
        void unload() override;
        void pause() override {};
        void resume() override {};
    };

    struct GameAssets {
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

        asset::Sprite* buttonSprite;
        asset::Sprite* buttonHoverSprite;
        asset::Sprite* buttonPressSprite;
        asset::SpriteAnimation* buttonPressAnim;

        asset::Sprite* workerButtonIcon;
        asset::Sprite* mineSiteButtonIcon;
        asset::Sprite* turretButtonIcon;

        asset::Sprite* digitSprites[10];
        asset::Sprite* stopwatchPunctuationSprite;

        asset::Sprite* mouseCursorSprite;

        asset::Sprite* overlaySprite;

        asset::Sound* backgroundMusic;
        asset::Sound* buttonHover;
        asset::Sound* resourceCollected;
        asset::Sound* resourceCollectedLow;
        asset::Sound* collect[3];
        asset::Sound* dropoff[3];
        asset::Sound* errorCollect;
        asset::Sound* resourceDropOff;
        asset::Sound* spawnMineSite;
        asset::Sound* turretShoot;
    };

    struct GameState : scene::Scene {
        uint32_t randomSeed;

        audio::PlayingSound* bgm;

        entity::Entity background;
        entity::Entity border;
        obstacle::Obstacle wallUp;
        obstacle::Obstacle wallDown;
        obstacle::Obstacle wallLeft;
        obstacle::Obstacle wallRight;

        base::Base base;
        mine_site::MineSiteSpawner mineSiteSpawner;
        entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES> mineSites;
        types::deque_arena<slurp::Vec2<float> > mineSpots;

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

        void load() override;
        void unload() override;
        void pause() override {};
        void resume() override {};
    };

    struct PauseMenuAssets {
        asset::Sprite* screenCoverSprite;
        asset::Sprite* pauseMenuSprite;
        asset::Sprite* resumeButtonTextSprite;
        asset::Sprite* exitButtonTextSprite;
        asset::Sprite* bigButtonSprite;
        asset::Sprite* bigButtonHoverSprite;
        asset::Sprite* bigButtonPressSprite;
        asset::Sound* buttonHover;
    };

    struct PauseMenuState: scene::Scene {
        ui::PauseMenu pauseMenu;

        void load() override;
        void unload() override {};
        void pause() override {};
        void resume() override {};
    };

    struct GameSystems {
        MainMenuAssets menuAssets;
        MainMenuState menuState;
        GameAssets assets;
        GameState state;
        PauseMenuAssets pauseMenuAssets;
        PauseMenuState pauseMenuState;
    };

    GLOBAL(MainMenuAssets* MenuAssets)
    GLOBAL(MainMenuState* MenuState)
    GLOBAL(GameAssets* Assets)
    GLOBAL(GameState* State)
    GLOBAL(PauseMenuAssets* PauseAssets)
    GLOBAL(PauseMenuState* PauseState)

    GLOBAL(bool mainMenuActive)
    GLOBAL(bool shouldTransitionScene)

    GLOBAL(memory::MemoryArena sceneMemory)

    void initialize(bool isInitialized);

    void handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    );

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState);

    void update(float dt);

    void frameEnd();

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target);

    void corruptWorkers(int numWorkers);

    void removeCorruptibleWorker(worker::Worker* worker);
}
