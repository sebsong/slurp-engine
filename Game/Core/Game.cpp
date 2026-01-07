#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"
#include "GameOverScreen.h"
#include "Scene.h"
#include "Settings.h"

#if UNITY_BUILD

#include "Obstacle.cpp"
#include "Base.cpp"
#include "GameOverScreen.cpp"
#include "MineSite.cpp"
#include "MineSiteSpawner.cpp"
#include "Worker.cpp"
#include "Turret.cpp"
#include "Button.cpp"
#include "MouseCursor.cpp"
#include "NumberDisplay.cpp"
#include "Stopwatch.cpp"
#include "ProgressBar.cpp"
#include "SpawnControls.cpp"

#endif

namespace game {
    static const float GlobalVolume = 1.0f;
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static constexpr const char* BackgroundMusicSoundFileName = "bgm.wav";

    static constexpr float EnableCorruptionDelay = 45.f;
    static constexpr int NumInitialCorruptedWorkers = 1;
    static constexpr float ResetHoldTime = 0.5f;

    static void loadAssets() {
        Assets->colorPalette = asset::loadColorPalette("slso8.hex");
        Assets->fontSmall = asset::loadFont("font_small.bmp");

        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");

        Assets->backgroundSprite = asset::loadSprite("main_menu.bmp");
        Assets->titleTextSprite = asset::loadSprite("title_text.bmp");
        Assets->madeWithSlurpEngineSprite = asset::loadSprite("made_with_slurp_engine.bmp");
        Assets->playButtonTextSprite = asset::loadSprite("play_button_text.bmp");
        Assets->resumeButtonTextSprite = asset::loadSprite("resume_button_text.bmp");
        Assets->mainMenuButtonTextSprite = asset::loadSprite("main_menu_button_text.bmp");
        Assets->exitButtonTextSprite = asset::loadSprite("exit_button_text.bmp");
        Assets->buttonSprite = asset::loadSprite("button_big.bmp");
        Assets->buttonHoverSprite = asset::loadSprite("button_big_hover.bmp");
        Assets->buttonPressSprite = asset::loadSprite("button_big_press.bmp");

        Assets->bgmIntro = asset::loadSound("bgm_chord_intro.wav", AUDIO_SOUND_GROUP_BGM);
        Assets->bgmMain = asset::loadSound("bgm_main.wav", AUDIO_SOUND_GROUP_BGM);
        Assets->buttonHoverSound = asset::loadSound("button_hover.wav");

        Assets->backgroundSprite = asset::loadSprite("background.bmp");
        Assets->borderSprite = asset::loadSprite("border.bmp");

        Assets->baseSprite = asset::loadSprite("base.bmp");
        Assets->baseIdleAnim = asset::loadSpriteAnimation("base_idle_anim.bmp", 5);

        Assets->mineSiteSprite = asset::loadSprite("gold.bmp");
        Assets->mineSiteSpawnAnim = asset::loadSpriteAnimation("gold_spawn.bmp", 9);

        Assets->workerSprite = asset::loadSprite("worker.bmp");
        Assets->workerLoadingAnim = asset::loadSpriteAnimation("worker_loading.bmp", 6);
        Assets->workerLoadedSprite = asset::loadSprite("worker_loaded.bmp");
        Assets->workerCorruptedSprite = asset::loadSprite("worker_corrupted.bmp");

        Assets->turretSprite = asset::loadSprite("turret.bmp");
        Assets->turretOrbSprite = asset::loadSprite("turret_orb.bmp");
        Assets->turretPitSprite = asset::loadSprite("turret_pit.bmp");
        Assets->turretRangeIndicatorSprite = asset::loadSprite("turret_range_indicator.bmp");
        Assets->turretRangeIndicatorSprite->material.alpha = 0.3f;
        Assets->turretSpawnAnim = asset::loadSpriteAnimation("turret_spawn_anim.bmp", 11);
        Assets->turretIdleAnim = asset::loadSpriteAnimation("turret_idle_anim.bmp", 6);
        Assets->turretShootAnim = asset::loadSpriteAnimation("turret_orb_absorb_anim.bmp", 8);

        Assets->resourcesCollectedFillSprite = asset::loadSprite("resources_collected_bar_fill.bmp");

        Assets->buttonSprite = asset::loadSprite("button.bmp");
        Assets->buttonHoverSprite = asset::loadSprite("button_hover.bmp");
        Assets->buttonPressSprite = asset::loadSprite("button_press.bmp");
        Assets->buttonPressAnim = asset::loadSpriteAnimation("button_press_anim.bmp", 15);

        Assets->workerButtonIcon = asset::loadSprite("worker_button_icon.bmp");
        Assets->mineSiteButtonIcon = asset::loadSprite("mine_site_button_icon.bmp");
        Assets->turretButtonIcon = asset::loadSprite("turret_button_icon.bmp");

        for (int i = 0; i < 10; i++) {
            Assets->digitSprites[i] = asset::loadSprite(std::format("{}.bmp", i));
        }
        Assets->stopwatchPunctuationSprite = asset::loadSprite("stopwatch.bmp");

        Assets->bgmChords = asset::loadSound("bgm_chords.wav", AUDIO_SOUND_GROUP_BGM);
        Assets->resourceCollected = asset::loadSound("resource_collected.wav");
        Assets->resourceCollectedLow = asset::loadSound("resource_collected_low.wav");
        Assets->collect[0] = asset::loadSound("collect_1_1.wav");
        Assets->collect[1] = asset::loadSound("collect_1_2.wav");
        Assets->collect[2] = asset::loadSound("collect_1_3.wav");
        Assets->dropoff[0] = asset::loadSound("dropoff_1_1.wav");
        Assets->dropoff[1] = asset::loadSound("dropoff_1_2.wav");
        Assets->dropoff[2] = asset::loadSound("dropoff_1_3.wav");
        Assets->errorCollect = asset::loadSound("error.wav");
        Assets->resourceDropOff = asset::loadSound("resource_drop_off.wav");
        Assets->spawnMineSite = asset::loadSound("spawn_mine_site.wav");
        Assets->turretShoot = asset::loadSound("turret_shoot.wav");

        Assets->screenCoverSprite = asset::loadSprite("screen_cover.bmp");
        Assets->pauseMenuSprite = asset::loadSprite("pause_menu.bmp");
        Assets->bigButtonSprite = asset::loadSprite("button_big.bmp");
        Assets->bigButtonHoverSprite = asset::loadSprite("button_big_hover.bmp");
        Assets->bigButtonPressSprite = asset::loadSprite("button_big_press.bmp");

        Assets->buttonHoverSound = asset::loadSound("button_hover.wav");

        Assets->gameOverScreenSprite = asset::loadSprite("game_over_screen.bmp");
    }

    void initialize(bool isInitialized) {
        if (!isInitialized) {
            sceneMemory = memory::Permanent->allocateSubArena("Scene Memory", sizeof(GameSystems));
        }
        GameSystems* gameSystems = sceneMemory.allocate<GameSystems>();
        Assets = &gameSystems->assets;
        GlobalScene = new(&gameSystems->globalScene) Global();
        MainMenuScene = new(&gameSystems->mainMenuScene) MainMenu();
        GameScene = new(&gameSystems->gameScene) Game();
        PauseMenuScene = new(&gameSystems->pauseMenuScene) PauseMenu();
        GameOverScene = new(&gameSystems->gameOverScene) GameOver();
        loadAssets();

        audio::setGlobalVolume(GlobalVolume);

        GameScene->randomSeed = static_cast<uint32_t>(time(nullptr));
        rnd::setRandomSeed(GameScene->randomSeed);

        render::setBackgroundColor(0.1f, 1.f, 0.2f);

        scene::registerScene(GlobalScene);
        scene::registerScene(MainMenuScene);
        scene::registerScene(GameScene);
        scene::registerScene(PauseMenuScene);
        scene::registerScene(GameOverScene);

        scene::start(GlobalScene);
        scene::start(MainMenuScene);
    }

    void Global::load() {
        new(&mouseCursor) mouse_cursor::MouseCursor(Assets->mouseCursorSprite);
        scene::registerEntity(this, &mouseCursor);
    }

    void MainMenu::load() {
        bgm = audio::play(
            Assets->bgmIntro,
            0.6,
            false,
            [this] {
                bgm = audio::play(Assets->bgmMain, 0.6, true);
            }
        );
        new(&background) entity::Entity(
            "Background",
            render::RenderInfo(render::SpriteInstance(Assets->backgroundSprite, BACKGROUND_Z)),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );
        scene::registerEntity(this, &background);

        new(&titleText) entity::Entity(
            "Title Text",
            render::RenderInfo(render::SpriteInstance(Assets->titleTextSprite, UI_Z)),
            physics::PhysicsInfo({0, 100}),
            collision::CollisionInfo()
        );
        scene::registerEntity(this, &titleText);

        new(&slurpEngineText) entity::Entity(
            "Slurp Engine Text",
            render::RenderInfo(render::SpriteInstance(Assets->madeWithSlurpEngineSprite, UI_Z)),
            physics::PhysicsInfo({275, -150}),
            collision::CollisionInfo()
        );
        scene::registerEntity(this, &slurpEngineText);

        new(&playButton) ui::Button(
            Assets->playButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -25},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                scene::transition(MainMenuScene, GameScene);
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2
        );
        scene::registerEntity(this, &playButton);

        new(&exitButton) ui::Button(
            Assets->exitButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -75},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                platform::exit();
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2
        );
        scene::registerEntity(this, &exitButton);

        testText = font::createText(Assets->fontSmall, "this is a test", {-75, 25}, UI_Z);
        scene::registerEntity(this, &testText);
    }

    void MainMenu::unload() {
        if (bgm) {
            audio::stop(bgm);
        }
    }

    void Game::load() {
        bgm = audio::play(Assets->bgmChords, 0.6, true);
        ignoreGoal = false;

        new(&background) entity::Entity(
            "Background",
            render::RenderInfo(render::SpriteInstance(Assets->backgroundSprite, BACKGROUND_Z)),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );
        scene::registerEntity(this, &background);

        new(&border) entity::Entity(
            "Border",
            render::RenderInfo(render::SpriteInstance(Assets->borderSprite, BORDER_Z)),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );
        scene::registerEntity(this, &border);

        new(&base) base::Base();
        scene::registerEntity(this, &base);

        new(&mineSiteSpawner) mine_site::MineSiteSpawner();
        scene::registerEntity(this, &mineSiteSpawner);

        new(&mineSites) entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES>(this, mine_site::MineSite());
        new(&mineSpots) types::deque_arena<slurp::Vec2<float> >();

        new(&workers) entity::EntityPool<worker::Worker, MAX_NUM_WORKERS>(this, worker::Worker());
        new(&corruptibleWorkers) types::vector_arena<worker::Worker*>();
        corruptionEnabled = false;
        timer::delay(
            EnableCorruptionDelay,
            [this] {
                corruptionEnabled = true;
                corruptWorkers(NumInitialCorruptedWorkers);
            }
        );

        new(&turrets) entity::EntityPool<turret::Turret, MAX_NUM_TURRETS>(this, turret::Turret());

        new(&spawnControls) ui::SpawnControls({0, -160});
        scene::registerEntity(this, &spawnControls);

        new(&goldProgressBar)
                ui::ProgressBar(
                    {0, 166},
                    0,
                    false,
                    nullptr,
                    Assets->resourcesCollectedFillSprite,
                    PROGRESS_BAR_Z
                );
        scene::registerEntity(this, &goldProgressBar);


        new(&stopwatch) ui::Stopwatch({280, 166});
        scene::registerEntity(this, &stopwatch);
        stopwatch.start();

        new(&resourcesCollectedDisplay)
                ui::NumberDisplay(
                    {-280, 166},
                    0,
                    MAX_NUM_DIGITS,
                    false
                );
        scene::registerEntity(this, &resourcesCollectedDisplay);

        resetTimer = timer::reserveHandle();
    }

    void Game::unload() {
        if (bgm) {
            audio::stop(bgm);
        }
    }

    void PauseMenu::load() {
        new(&menu) entity::Entity(
            "PauseMenu",
            render::RenderInfo{
                (render::SpriteInstance[2]){
                    render::SpriteInstance(
                        Assets->screenCoverSprite,
                        MENU_Z + 1
                    ),
                    render::SpriteInstance(
                        Assets->pauseMenuSprite,
                        MENU_Z
                    )
                }
            },
            physics::PhysicsInfo{},
            collision::CollisionInfo{}
        );
        menu.setAlpha(0, .75);
        scene::registerEntity(this, &menu);

        new(&stopwatch) ui::Stopwatch(
            {20, 25},
            MENU_Z - 1,
            GameScene->stopwatch.getSecondsElapsed()
        );
        stopwatch.setColor(Assets->colorPalette->at(2));
        scene::registerEntity(this, &stopwatch);

        new(&resumeButton) ui::Button(
            Assets->resumeButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -15},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                scene::end(PauseMenuScene);
                scene::resume(GameScene);
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &resumeButton);

        new(&mainMenuButton) ui::Button(
            Assets->mainMenuButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -55},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                scene::end(PauseMenuScene);
                scene::end(GameOverScene);
                scene::end(GameScene);
                scene::start(MainMenuScene);
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &mainMenuButton);

        new(&exitButton) ui::Button(
            Assets->exitButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -95},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                platform::exit();
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &exitButton);
    }

    void GameOver::load() {
        new(&gameOverScreen) ui::GameOverScreen();
        scene::registerEntity(this, &gameOverScreen);

        new(&stopwatch) ui::Stopwatch(
            {20, 0},
            MENU_Z - 1,
            GameScene->stopwatch.getSecondsElapsed()
        );
        stopwatch.setColor(Assets->colorPalette->at(2));
        scene::registerEntity(this, &stopwatch);

        new(&resumeButton) ui::Button(
            Assets->resumeButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {-120, -75},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                scene::end(GameOverScene);
                GameScene->ignoreGoal = true;
                GameScene->stopwatch.start();
                scene::resume(GameScene);
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &resumeButton);

        new(&mainMenuButton) ui::Button(
            Assets->mainMenuButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {0, -75},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                scene::end(GameOverScene);
                scene::end(GameScene);
                scene::start(MainMenuScene);
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &mainMenuButton);

        new(&exitButton) ui::Button(
            Assets->exitButtonTextSprite,
            Assets->bigButtonSprite,
            Assets->bigButtonHoverSprite,
            Assets->bigButtonPressSprite,
            BigButtonShape,
            {120, -75},
            std::nullopt,
            [](ui::Button* _) {},
            [](ui::Button* _) {
                platform::exit();
            },
            [](ui::Button* _) {},
            [](ui::Button* _) {
                audio::play(Assets->buttonHoverSound);
            },
            -2,
            MENU_Z - 1
        );
        scene::registerEntity(this, &exitButton);
    }

    static void endGame() {
        GameScene->stopwatch.stop();
        scene::pause(GameScene);
        scene::end(PauseMenuScene);
        scene::start(GameOverScene);
    }

    void handleMouseAndKeyboardInput(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState) {
        if (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) {
            platform::exit();
        }

        if (keyboardState.justPressed(slurp::KeyboardCode::TAB)) {
            if (MainMenuScene->isActive) {
                scene::transition(MainMenuScene, GameScene);
            } else {
                scene::transition(GameScene, MainMenuScene);
            }
        }

        if (GameScene->isActive && !GameOverScene->isActive) {
            if (keyboardState.justPressed(slurp::KeyboardCode::ESCAPE)) {
                if (!GameScene->isPaused) {
                    scene::pause(GameScene);
                    scene::start(PauseMenuScene);
                } else {
                    scene::end(PauseMenuScene);
                    scene::resume(GameScene);
                }
            }

            if (keyboardState.justPressed(slurp::KeyboardCode::R)) {
                timer::start(
                    GameScene->resetTimer,
                    ResetHoldTime,
                    false,
                    [] {
                        scene::end(GameScene);
                        scene::start(GameScene);
                    }
                );
            } else if (!keyboardState.isDown(slurp::KeyboardCode::R)) {
                timer::cancel(GameScene->resetTimer);
            }

#if DEBUG
            if (keyboardState.justPressed(slurp::KeyboardCode::W)) {
                endGame();
            }
#endif
        }
    }

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            platform::exit();
        }
    }

    void update(float dt) {
        float goldProgress = GameScene->base.getProgress();
        GameScene->goldProgressBar.progress = goldProgress;
        if (!GameScene->ignoreGoal && goldProgress >= 1.f) {
            endGame();
        }
    }

    void frameEnd() {}

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target) {
        return entity->physicsInfo.position.distanceSquaredTo(target) < entity->physicsInfo.speed * 0.01f;
    }

    void corruptWorkers(int numWorkers) {
        if (GameScene->corruptibleWorkers.empty()) {
            return;
        }
        rnd::shuffle(GameScene->corruptibleWorkers);
        std::vector targetWorkers = std::vector(
            GameScene->corruptibleWorkers.begin(),
            GameScene->corruptibleWorkers.begin() + numWorkers
        );
        for (worker::Worker* target: targetWorkers) {
            target->corrupt();
        }
    }

    void removeCorruptibleWorker(worker::Worker* worker) {
        auto position = std::ranges::find(GameScene->corruptibleWorkers, worker);
        if (position != GameScene->corruptibleWorkers.end()) {
            GameScene->corruptibleWorkers.erase(position);
        }
    }
}
