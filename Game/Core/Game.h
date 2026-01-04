#pragma once
#include "AudioPlayer.h"
#include "Base.h"
#include "EntityPool.h"
#include "GameOverScreen.h"
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
#include "Stopwatch.h"

#define MAX_NUM_WORKERS 10000
#define MAX_NUM_TURRETS 1000
#define MAX_NUM_MINE_SITES 1000

namespace game {
    struct GameAssets {
        asset::Sprite* backgroundSprite;
        asset::Sprite* mouseCursorSprite;

        asset::Sprite* titleTextSprite;
        asset::Sprite* madeWithSlurpEngineSprite;
        asset::Sprite* playButtonTextSprite;
        asset::Sprite* exitButtonTextSprite;
        asset::Sprite* bigButtonSprite;
        asset::Sprite* bigButtonHoverSprite;
        asset::Sprite* bigButtonPressSprite;

        asset::Sound* bgmIntro;
        asset::Sound* bgmMain;
        asset::Sound* buttonHoverSound;

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
        asset::Sprite* resourcesCollectedFillSprite;
        asset::Sprite* buttonSprite;
        asset::Sprite* buttonHoverSprite;
        asset::Sprite* buttonPressSprite;
        asset::SpriteAnimation* buttonPressAnim;
        asset::Sprite* workerButtonIcon;
        asset::Sprite* mineSiteButtonIcon;
        asset::Sprite* turretButtonIcon;
        asset::Sprite* digitSprites[10];
        asset::Sprite* stopwatchPunctuationSprite;

        asset::Sound* bgmChords;
        asset::Sound* resourceCollected;
        asset::Sound* resourceCollectedLow;
        asset::Sound* collect[3];
        asset::Sound* dropoff[3];
        asset::Sound* errorCollect;
        asset::Sound* resourceDropOff;
        asset::Sound* spawnMineSite;
        asset::Sound* turretShoot;

        asset::Sprite* screenCoverSprite;
        asset::Sprite* pauseMenuSprite;

        asset::Sprite* gameOverScreenSprite;
    };

    struct Global : scene::Scene {
        mouse_cursor::MouseCursor mouseCursor;

        void load() override;

        void unload() override {};
        void pause() override {};
        void resume() override {};
    };

    struct MainMenu : scene::Scene {
        audio::PlayingSound* bgm;

        entity::Entity background;
        entity::Entity titleText;
        entity::Entity slurpEngineText;
        ui::UIButton playButton;
        ui::UIButton exitButton;

        void load() override;

        void unload() override;

        void pause() override {};
        void resume() override {};
    };

    struct Game : scene::Scene {
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
        ui::Stopwatch stopwatch;

        entity::Entity overlay;

        timer::timer_handle resetTimer;

        void load() override;

        void unload() override;

        void pause() override {};
        void resume() override {};
    };

    struct PauseMenu : scene::Scene {
        ui::PauseMenu pauseMenu;

        void load() override;

        void unload() override {};
        void pause() override {};
        void resume() override {};
    };

    struct GameOver : scene::Scene {
        ui::GameOverScreen gameOverScreen;

        void load() override;

        void unload() override {};
        void pause() override {};
        void resume() override {};
    };

    struct GameSystems {
        GameAssets assets;
        Global globalScene;
        MainMenu mainMenuScene;
        Game gameScene;
        PauseMenu pauseMenuScene;
        GameOver gameOverScene;
    };

    GLOBAL(GameAssets* Assets)
    GLOBAL(Global* GlobalScene)
    GLOBAL(MainMenu* MainMenuScene)
    GLOBAL(Game* GameScene)
    GLOBAL(PauseMenu* PauseMenuScene)
    GLOBAL(GameOver* GameOverScene)

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
