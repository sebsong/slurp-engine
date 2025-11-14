#include "Game.h"

#include "SlurpEngine.h"
#include "RenderApi.h"
#include "EntityPipeline.h"
#include "Settings.h"

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

namespace game {
    static const float GlobalVolume = 0.3f;
    // NOTE: https://freesound.org/people/Seth_Makes_Sounds/sounds/706018/
    // static constexpr const char* BackgroundMusicSoundFileName = "bgm.wav";

    static constexpr float EnableCorruptionDelay = 45.f;
    static constexpr int NumInitialCorruptedWorkers = 1;

    static void loadAssets() {
        Assets->backgroundSprite = asset::loadSprite("background.bmp");
        Assets->borderSprite = asset::loadSprite("border.bmp");

        Assets->baseSprite = asset::loadSprite("base.bmp");
        Assets->mineSiteSprite = asset::loadSprite("gold.bmp");
        Assets->mineSiteSpawnAnim = asset::loadSpriteAnimation("gold_spawn.bmp", 9);

        Assets->workerSprite = asset::loadSprite("worker.bmp");
        Assets->workerLoadingAnim = asset::loadSpriteAnimation("worker_loading.bmp", 6);
        Assets->workerLoadedSprite = asset::loadSprite("worker_loaded.bmp");
        Assets->workerCorruptedSprite = asset::loadSprite("worker_corrupted.bmp");

        Assets->turretSprite = asset::loadSprite("turret.bmp");
        Assets->turretRangeIndicatorSprite = asset::loadSprite("turret_range_indicator.bmp");
        Assets->turretRangeIndicatorSprite->material.alpha = 0.3f;
        Assets->turretSpawnAnim = asset::loadSpriteAnimation("turret_spawn_anim.bmp", 8);
        Assets->turretIdleAnim = asset::loadSpriteAnimation("turret_idle_anim.bmp", 6);
        Assets->turretShootAnim = asset::loadSpriteAnimation("turret_shoot.bmp", 1);

        Assets->resourcesCollectedFill = asset::loadSprite("resources_collected_bar_fill.bmp");

        Assets->workerButton = asset::loadSprite("worker_button.bmp");
        Assets->workerButtonHover = asset::loadSprite("worker_button_hover.bmp");
        Assets->workerButtonPress = asset::loadSprite("worker_button_pressed.bmp");
        Assets->workerButtonPressAnim = asset::loadSpriteAnimation("worker_button_pressed_anim.bmp", 15);

        Assets->mineSiteButton = asset::loadSprite("mine_site_button.bmp");
        Assets->mineSiteButtonHover = asset::loadSprite("mine_site_button_hover.bmp");
        Assets->mineSiteButtonPress = asset::loadSprite("mine_site_button_pressed.bmp");
        Assets->mineSiteButtonPressAnim = asset::loadSpriteAnimation("mine_site_button_pressed_anim.bmp", 15);

        Assets->turretButton = asset::loadSprite("mine_site_button.bmp");
        Assets->turretButtonHover = asset::loadSprite("mine_site_button_hover.bmp");
        Assets->turretButtonPress = asset::loadSprite("mine_site_button_pressed.bmp");

        for (int i = 0; i < 10; i++) {
            Assets->digitSprites[i] = asset::loadSprite(std::format("{}.bmp", i));
        }
        Assets->stopwatchPunctuationSprite = asset::loadSprite("stopwatch.bmp");

        Assets->mouseCursorSprite = asset::loadSprite("mouse_cursor.bmp");

        // NOTE: https://opengameart.org/content/since-2-am
        Assets->backgroundMusic = asset::loadSound("since_2_am.wav");
        Assets->resourceCollected = asset::loadSound("resource_collected.wav");
        Assets->resourceCollectedLow = asset::loadSound("resource_collected_low.wav");
        Assets->errorCollect = asset::loadSound("error.wav");
        Assets->resourceDropOff = asset::loadSound("resource_drop_off.wav");
        Assets->spawnMineSite = asset::loadSound("spawn_mine_site.wav");
        Assets->turretShoot = asset::loadSound("turret_shoot.wav");
    }

    void initialize(bool isInitialized) {
        if (isInitialized) {
            Assets = slurp::Globals->GameAssets;
            State = slurp::Globals->GameState;
        } else {
            GameSystems* gameSystems = memory::Permanent->allocate<GameSystems>();
            Assets = slurp::Globals->GameAssets = &gameSystems->assets;
            State = slurp::Globals->GameState = &gameSystems->state;
            loadAssets();
        }

        State->randomSeed = static_cast<uint32_t>(time(nullptr));
        random::setRandomSeed(State->randomSeed);

        audio::setGlobalVolume(GlobalVolume);
        if (!State->bgmId) {
            State->bgmId = audio::play(Assets->backgroundMusic, 0.5, true);
        }
        slurp::Globals->RenderApi->setBackgroundColor(0.4f, 0.1f, 1.0f);

        new(&State->background) entity::Entity(
            "Background",
            render::RenderInfo(slurp::Globals->GameAssets->backgroundSprite, true, BACKGROUND_Z),
            physics::PhysicsInfo(),
            collision::CollisionInfo()
        );

        new(&State->border) entity::Entity(
            "Border",
            render::RenderInfo(slurp::Globals->GameAssets->borderSprite, true, BORDER_Z),
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
        new(&State->turretsRangeIndicators) entity::EntityPool<entity::Entity, MAX_NUM_TURRETS>(
            entity::Entity(
                "Turret Range Indicator",
                render::RenderInfo(
                    Assets->turretRangeIndicatorSprite,
                    true,
                    BACKGROUND_ENTITY_Z,
                    turret::RenderOffset
                ),
                physics::PhysicsInfo(),
                collision::CollisionInfo()
            )
        );

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

        new(&State->mouseCursor) mouse_cursor::MouseCursor();
    }

    void handleMouseAndKeyboardInput(const slurp::MouseState& mouseState, const slurp::KeyboardState& keyboardState) {
        if (
            (keyboardState.isDown(slurp::KeyboardCode::ALT) && keyboardState.isDown(slurp::KeyboardCode::F4)) ||
            keyboardState.isDown(slurp::KeyboardCode::ESC)
        ) {
            slurp::Globals->PlatformDll->shutdown();
        }
    }

    void handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {
        if (gamepadState.isDown(slurp::GamepadCode::START) || gamepadState.isDown(slurp::GamepadCode::B)) {
            slurp::Globals->PlatformDll->shutdown();
        }
    }

    void update(float dt) {
        State->goldProgressBar.progress = State->base.getProgress();
    }

    bool almostAtTarget(entity::Entity* entity, slurp::Vec2<float> target) {
        return entity->physicsInfo.position.distanceSquaredTo(target) < entity->physicsInfo.speed * 0.01f;
    }

    void corruptWorkers(int numWorkers) {
        random::shuffle(State->corruptibleWorkers);
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
