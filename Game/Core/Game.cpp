#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"
#include "Settings.h"

#if UNITY_BUILD

#include "Obstacle.cpp"
#include "Base.cpp"
#include "MineSite.cpp"
#include "MineSiteSpawner.cpp"
#include "Worker.cpp"
#include "Turret.cpp"
#include "UIButton.cpp"
#include "MouseCursor.cpp"
#include "NumberDisplay.cpp"
#include "StopwatchDisplay.cpp"
#include "ProgressBar.cpp"
#include "SpawnControls.cpp"
#include "PauseMenu.cpp"

#endif

namespace game {
    static const float GlobalVolume = 1.0f;
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static constexpr const char* BackgroundMusicSoundFileName = "bgm.wav";

    static constexpr float EnableCorruptionDelay = 45.f;
    static constexpr int NumInitialCorruptedWorkers = 1;

    static void loadAssets() {
        MenuAssets->backgroundSprite = asset::loadSprite("main_menu.bmp");
        MenuAssets->titleTextSprite = asset::loadSprite("title_text.bmp");
        MenuAssets->playButtonTextSprite = asset::loadSprite("play_button_text.bmp");
        MenuAssets->exitButtonTextSprite = asset::loadSprite("exit_button_text.bmp");
        MenuAssets->buttonSprite = asset::loadSprite("button_big.bmp");
        MenuAssets->buttonHoverSprite = asset::loadSprite("button_big_hover.bmp");
        MenuAssets->buttonPressSprite = asset::loadSprite("button_big_press.bmp");

        MenuAssets->bgmIntro = asset::loadSound("bgm_chord_intro.wav", AUDIO_SOUND_GROUP_BGM);
        MenuAssets->bgmMain = asset::loadSound("bgm_main.wav", AUDIO_SOUND_GROUP_BGM);
        MenuAssets->buttonHover = asset::loadSound("button_hover.wav");

        Assets->backgroundSprite = asset::loadSprite("background.bmp");
        Assets->borderSprite = asset::loadSprite("border.bmp");

        Assets->screenCoverSprite = asset::loadSprite("screen_cover.bmp");
        Assets->pauseMenuSprite = asset::loadSprite("pause_menu.bmp");
        Assets->resumeButtonTextSprite = asset::loadSprite("play_button_text.bmp");
        Assets->exitButtonTextSprite = asset::loadSprite("exit_button_text.bmp");
        Assets->bigButtonSprite = asset::loadSprite("button_big.bmp");
        Assets->bigButtonHoverSprite = asset::loadSprite("button_big_hover.bmp");
        Assets->bigButtonPressSprite = asset::loadSprite("button_big_press.bmp");

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

        Assets->resourcesCollectedFill = asset::loadSprite("resources_collected_bar_fill.bmp");

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

        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");

        Assets->overlaySprite = asset::loadSprite("overlay.bmp", "overlay.glsl", "overlay.glsl");

        Assets->backgroundMusic = asset::loadSound("bgm_chords.wav", AUDIO_SOUND_GROUP_BGM);
        Assets->buttonHover = asset::loadSound("button_hover.wav");
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
    }

    static void transitionScene(bool isMainMenu) {
        mainMenuActive = isMainMenu;
        audio::clearAll();
        entity::clearAll();
        sceneMemory.freeAll();
        initialize(true);
    }

    void initialize(bool isInitialized) {
        if (!isInitialized) {
            sceneMemory = memory::Permanent->allocateSubArena("Scene Memory", sizeof(GameSystems));
            mainMenuActive = true;
        }
        GameSystems* gameSystems = sceneMemory.allocate<GameSystems>();
        MenuAssets = slurp::Globals->MenuAssets = &gameSystems->menuAssets;
        MenuState = slurp::Globals->MenuState = &gameSystems->menuState;
        Assets = slurp::Globals->GameAssets = &gameSystems->assets;
        State = slurp::Globals->GameState = &gameSystems->state;
        loadAssets();

        audio::setGlobalVolume(GlobalVolume);

        State->randomSeed = static_cast<uint32_t>(time(nullptr));
        rnd::setRandomSeed(State->randomSeed);

        slurp::Globals->RenderApi->setBackgroundColor(0.1f, 1.f, 0.2f);

        if (mainMenuActive) {
            audio::play(
                MenuAssets->bgmIntro,
                0.6,
                false,
                [] {
                    audio::play(MenuAssets->bgmMain, 0.6, true);
                }
            );
            new(&MenuState->background) entity::Entity(
                "Background",
                render::RenderInfo(render::SpriteInstance(MenuAssets->backgroundSprite, BACKGROUND_Z)),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            );
            new(&MenuState->titleText) entity::Entity(
                "Title Text",
                render::RenderInfo(render::SpriteInstance(MenuAssets->titleTextSprite, UI_Z)),
                physics::PhysicsInfo({0, 100}),
                collision::CollisionInfo()
            );
            const geometry::Shape& buttonShape = geometry::Shape(geometry::Rect, {52, 34});
            new(&MenuState->playButton) ui::UIButton(
                MenuAssets->playButtonTextSprite,
                MenuAssets->buttonSprite,
                MenuAssets->buttonHoverSprite,
                MenuAssets->buttonPressSprite,
                buttonShape,
                {0, -25},
                std::nullopt,
                [](ui::UIButton* _) {},
                [](ui::UIButton* _) {
                    transitionScene(false);
                },
                [](ui::UIButton* _) {},
                [](ui::UIButton* _) {
                    audio::play(Assets->buttonHover);
                },
                -2
            );

            new(&MenuState->exitButton) ui::UIButton(
                MenuAssets->exitButtonTextSprite,
                MenuAssets->buttonSprite,
                MenuAssets->buttonHoverSprite,
                MenuAssets->buttonPressSprite,
                buttonShape,
                {0, -75},
                std::nullopt,
                [](ui::UIButton* _) {},
                [](ui::UIButton* _) {
                    platform::exit();
                },
                [](ui::UIButton* _) {},
                [](ui::UIButton* _) {
                    audio::play(Assets->buttonHover);
                },
                -2
            );
            new(&State->mouseCursor) mouse_cursor::MouseCursor();
            return;
        }

        audio::play(Assets->backgroundMusic, 0.6, true);

        new(&State->background) entity::Entity(
            "Background",
            render::RenderInfo(render::SpriteInstance(Assets->backgroundSprite, BACKGROUND_Z)),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );

        new(&State->border) entity::Entity(
            "Border",
            render::RenderInfo(render::SpriteInstance(Assets->borderSprite, BORDER_Z)),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );

        new(&State->base) base::Base();

        new(&State->mineSiteSpawner) mine_site::MineSiteSpawner();

        new(&State->mineSites) entity::EntityPool<mine_site::MineSite, MAX_NUM_MINE_SITES>(mine_site::MineSite());
        new(&State->mineSpots) types::deque_arena<slurp::Vec2<float> >();

        new(&State->workers) entity::EntityPool<worker::Worker, MAX_NUM_WORKERS>(worker::Worker());
        new(&State->corruptibleWorkers) types::vector_arena<worker::Worker*>();
        State->corruptionEnabled = false;
        timer::delay(
            EnableCorruptionDelay,
            [] {
                State->corruptionEnabled = true;
                corruptWorkers(NumInitialCorruptedWorkers);
            }
        );

        new(&State->turrets) entity::EntityPool<turret::Turret, MAX_NUM_TURRETS>(turret::Turret());

        new(&State->spawnControls) ui::SpawnControls({0, -160});

        new(&State->goldProgressBar)
                ui::ProgressBar(
                    {0, 166},
                    0,
                    false,
                    nullptr,
                    Assets->resourcesCollectedFill,
                    PROGRESS_BAR_Z
                );


        new(&State->stopwatchDisplay) ui::StopwatchDisplay({280, 166});

        new(&State->resourcesCollectedDisplay)
                ui::NumberDisplay(
                    {-280, 166},
                    0,
                    MAX_NUM_DIGITS,
                    false
                );

        new(&State->pauseMenu) ui::PauseMenu();

        new(&State->mouseCursor) mouse_cursor::MouseCursor();
        // new(&State->overlay) entity::Entity(
        //     "Overlay",
        //     render::RenderInfo(render::SpriteInstance(Assets->overlaySprite, -Z_ORDER_MAX)),
        //     physics::PhysicsInfo(),
        //     collision::CollisionInfo()
        // );
    }

    void handleMouseAndKeyboardInput(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState) {
        if (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) {
            platform::exit();
        }

        if (keyboardState.justPressed(slurp::KeyboardCode::TAB)) {
            transitionScene(!mainMenuActive);
        }

        if (!mainMenuActive && keyboardState.justPressed(slurp::KeyboardCode::ESCAPE)) {
            State->pauseMenu.toggle();
        }
    }

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            platform::exit();
        }
    }

    void update(float dt) {
        State->goldProgressBar.progress = State->base.getProgress();
    }

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target) {
        return entity->physicsInfo.position.distanceSquaredTo(target) < entity->physicsInfo.speed * 0.01f;
    }

    void corruptWorkers(int numWorkers) {
        if (State->corruptibleWorkers.empty()) {
            return;
        }
        rnd::shuffle(State->corruptibleWorkers);
        std::vector targetWorkers = std::vector(
            State->corruptibleWorkers.begin(),
            State->corruptibleWorkers.begin() + numWorkers
        );
        for (worker::Worker* target: targetWorkers) {
            target->corrupt();
        }
    }

    void removeCorruptibleWorker(worker::Worker* worker) {
        auto position = std::ranges::find(State->corruptibleWorkers, worker);
        if (position != State->corruptibleWorkers.end()) {
            game::State->corruptibleWorkers.erase(position);
        }
    }
}
