#include "SlurpEngine.h"
#include "Random.h"
#include "Debug.h"

#include <iostream>

/* Single translation unit, unity build */
// ReSharper disable once CppUnusedIncludeDirective
#include "Update.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Collision.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Render.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "UpdateRenderPipeline.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Timer.cpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "Game.cpp"

typedef unsigned char byte;

namespace slurp {
    static platform::PlatformDll GlobalPlatformDll;
    static GameState* GlobalGameState;
    static UpdateRenderPipeline* GlobalUpdateRenderPipeline;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static constexpr int MouseCursorSizePixels = 10;
    static constexpr render::ColorPaletteIdx MouseCursorColorPalletIdx = 1;

    static const Vector2<int> PlayerStartPos = {640, 360};
    static constexpr int BasePlayerSizePixels = 20;
    static constexpr render::ColorPaletteIdx PlayerColorPalletIdx = 3;
    static constexpr render::ColorPaletteIdx PlayerParryColorPalletIdx = 0;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;

    static constexpr float ParryActiveDuration = .1f;

    static const Vector2<int> EnemyStartPos = {400, 200};
    static const Vector2<int> EnemyPosOffset = {100, 0};
    static constexpr int BaseEnemySizePixels = 20;
    static constexpr render::ColorPaletteIdx EnemyColorPalletIdx = 4;
    static constexpr int BaseEnemySpeed = 200;
    static constexpr float BaseEnemyDirectionChangeDelay = 2;
    static constexpr float EnemyDirectionChangeDelayDelta = 1.5;

    static constexpr int ProjectileSizePixels = 15;
    static constexpr render::ColorPaletteIdx ProjectileColorPalletIdx = 1;
    static constexpr int BaseProjectileSpeed = 500;

    static void drawColorPaletteSwatch(render::GraphicsBuffer buffer, Vector2<int> point, int size) {
        Vector2<int> position = point;
        for (uint8_t i = 0; i < COLOR_PALETTE_SIZE; i++) {
            drawSquare(
                buffer,
                position,
                size,
                i,
                GlobalGameState->colorPalette
            );
            position.x += size;
        }
    }

    static void setRandomDirection(Entity* entity) {
        float randX = random::randomFloat(-1, 1);
        float randY = random::randomFloat(-1, 1);
        entity->direction = Vector2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay() {
        float minDelay = BaseEnemyDirectionChangeDelay - EnemyDirectionChangeDelayDelta;
        float maxDelay = BaseEnemyDirectionChangeDelay + EnemyDirectionChangeDelayDelta;
        return random::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(Entity* enemy) {
        setRandomDirection(enemy);
        timer::delay(getRandomDirectionChangeDelay(), [&] {
            startUpdateEnemyDirection(enemy);
        });
    }

    SLURP_INIT(init) {
        GlobalPlatformDll = platformDll;

        assert(sizeof(MemorySections) <= gameMemory->permanentMemory.sizeBytes);
        MemorySections* sections = static_cast<MemorySections*>(gameMemory->permanentMemory.memory);

        render::ColorPalette colorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);

        new(&sections->updateRenderPipeline) UpdateRenderPipeline(colorPalette);
        GlobalUpdateRenderPipeline = &sections->updateRenderPipeline;

        GlobalGameState = &sections->gameState;
        GlobalGameState->colorPalette = colorPalette;
        GlobalGameState->randomSeed = static_cast<uint32_t>(time(nullptr));

        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->background,
            "Background",
            {0, 0},
            {geometry::Rect, {1280, 720}},
            7,
            false
        );

        geometry::Shape wallUpShape = {geometry::Rect, {1500, 20}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->wallUp,
            "WallUp",
            {0, 0},
            wallUpShape,
            5,
            false
        ).enableCollision(true, wallUpShape, false);

        geometry::Shape wallDownShape = {geometry::Rect, {1500, 20}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->wallDown,
            "WallDown",
            {0, 700},
            wallDownShape,
            5,
            false
        ).enableCollision(true, wallDownShape, false);

        geometry::Shape wallLeftShape = {geometry::Rect, {20, 1000}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->wallLeft,
            "WallLeft",
            {0, 0},
            wallLeftShape,
            5,
            false
        ).enableCollision(true, wallLeftShape, false);

        geometry::Shape wallRightShape = {geometry::Rect, {20, 1000}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->wallRight,

            "WallRight",
            {1260, 0},
            wallRightShape,
            5,
            false
        ).enableCollision(true, wallRightShape, false);

        geometry::Shape obstacle1Shape = {geometry::Rect, {150, 150}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->obstacle1,

            "Obstacle1",
            {200, 500},
            obstacle1Shape,
            5,
            true
        ).enableCollision(true, obstacle1Shape, true);

        geometry::Shape obstacle2Shape = {geometry::Rect, {300, 200}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->obstacle2,

            "Obstacle2",
            {500, 500},
            obstacle2Shape,
            5,
            true
        ).enableCollision(true, obstacle2Shape, true);

        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->mouseCursor,
            "MouseCursor",
            {},
            {geometry::Rect, {MouseCursorSizePixels, MouseCursorSizePixels}},
            MouseCursorColorPalletIdx,
            true
        );

        geometry::Shape playerShape = {geometry::Rect, {BasePlayerSizePixels, BasePlayerSizePixels}};
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->player,
            "Player",
            PlayerStartPos,
            playerShape,
            PlayerColorPalletIdx,
            true
        );
        GlobalGameState->player.speed = BasePlayerSpeed;
        GlobalGameState->player.enableCollision(
            false,
            playerShape,
            true,
            [](const Entity* otherEntity) {
                std::cout << "ENTER: " << otherEntity->name << std::endl;
            },
            [](const Entity* otherEntity) {
                std::cout << "EXIT: " << otherEntity->name << std::endl;
            }
        );

        geometry::Shape enemyShape = {geometry::Rect, {BaseEnemySizePixels, BaseEnemySizePixels}};
        for (int i = 0; i < NUM_ENEMIES; i++) {
            Entity& enemy = GlobalGameState->enemies[i];
            GlobalUpdateRenderPipeline->initAndRegister(
                enemy,
                "Enemy" + std::to_string(i),
                EnemyStartPos + (EnemyPosOffset * i),
                enemyShape,
                EnemyColorPalletIdx,
                true
            );
            enemy.speed = BaseEnemySpeed;
            enemy.enableCollision(false, enemyShape, true);
            // startUpdateEnemyDirection(enemy); // TODO: re-enable this
        }

        geometry::Shape projectileShape = {geometry::Rect, {ProjectileSizePixels, ProjectileSizePixels}};
        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            Entity& projectile = GlobalGameState->projectiles[i];
            GlobalUpdateRenderPipeline->initAndRegister(
                projectile,
                "Projectile" + std::to_string(i),
                {},
                projectileShape,
                ProjectileColorPalletIdx,
                true
            );
            projectile.enabled = false;
            projectile.speed = BaseProjectileSpeed;
            projectile.enableCollision(
                false,
                projectileShape,
                true,
                [](const Entity* other) {
                    if (const Player* player = dynamic_cast<const Player*>(other)) {
                        std::cout << "PLAYER HIT: " << player->name << std::endl;
                    } else {
                        std::cout << "OTHER HIT: " << other->name << std::endl;
                    }
                },
                [](const Entity* other) {
                    if (const Player* player = dynamic_cast<const Player*>(other)) {
                        std::cout << "PLAYER EXIT: " << player->name << std::endl;
                    } else {
                        std::cout << "OTHER EXIT: " << other->name << std::endl;
                    }
                }
                );
        }

        if (!GlobalGameState->isInitialized) {
            // NOTE: anything that shouldn't be hot reloaded goes here
            GlobalGameState->isInitialized = true;
        }

        random::setRandomSeed(GlobalGameState->randomSeed);

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
    }

    static void activateParry(Player& player) {
        player.isParryActive = true;
        player.renderShape.color = GlobalGameState->colorPalette.colors[PlayerParryColorPalletIdx];
    }

    static void deactivateParry(Player& player) {
        player.isParryActive = false;
        player.renderShape.color = GlobalGameState->colorPalette.colors[PlayerColorPalletIdx];
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput) {
        GlobalGameState->mouseCursor.position = mouseState.position;

        if (mouseState.justPressed(MouseCode::LeftClick)) {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.position;
            projectile.direction =
                    static_cast<Vector2<float>>(mouseState.position - GlobalGameState->player.position).
                    normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) {
                GlobalGameState->projectileIdx = 0;
            }
        }

        if (mouseState.justPressed(MouseCode::RightClick) || keyboardState.justPressed(KeyboardCode::E)) {
            activateParry(GlobalGameState->player);
            timer::delay(ParryActiveDuration, [] { deactivateParry(GlobalGameState->player); });
        }

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4)) {
            GlobalPlatformDll.shutdown();
        }

        Vector2<float> direction;
        if (keyboardState.isDown(KeyboardCode::W)) {
            direction.y -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::A)) {
            direction.x -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::S)) {
            direction.y += 1;
        }
        if (keyboardState.isDown(KeyboardCode::D)) {
            direction.x += 1;
        }
        GlobalGameState->player.direction = direction.normalize();


        if (keyboardState.justPressed(KeyboardCode::SPACE)) {
            GlobalGameState->player.speed = SprintPlayerSpeed;
        } else if (keyboardState.justReleased(KeyboardCode::SPACE)) {
            GlobalGameState->player.speed = BasePlayerSpeed;
        }

#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P)) {
            GlobalPlatformDll.DEBUG_togglePause();
        }
        if (keyboardState.justPressed(KeyboardCode::R) && !GlobalRecordingState->isPlayingBack) {
            if (!GlobalRecordingState->isRecording) {
                GlobalRecordingState->isRecording = true;
                GlobalPlatformDll.DEBUG_beginRecording();
            } else {
                GlobalPlatformDll.DEBUG_endRecording();
                GlobalRecordingState->isRecording = false;
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T)) {
            GlobalRecordingState->isPlayingBack = true;
            auto onPlaybackEnd = []() -> void { GlobalRecordingState->isPlayingBack = false; };
            GlobalPlatformDll.DEBUG_beginPlayback(onPlaybackEnd);
        }
#endif

        if (keyboardState.isDown(KeyboardCode::ESC)) {
            GlobalPlatformDll.shutdown();
        }
    }

    SLURP_HANDLE_GAMEPAD_INPUT(handleGamepadInput) {
        for (int controllerIdx = 0; controllerIdx < MAX_NUM_CONTROLLERS; controllerIdx++) {
            GamepadState gamepadState = controllerStates[controllerIdx];
            if (!gamepadState.isConnected) {
                continue;
            }

            if (gamepadState.isDown(GamepadCode::START) || gamepadState.isDown(GamepadCode::B)) {
                GlobalPlatformDll.shutdown();
            }

            if (gamepadState.justPressed(GamepadCode::LEFT_SHOULDER) || gamepadState.justPressed(
                    GamepadCode::RIGHT_SHOULDER)) {
                GlobalGameState->player.speed = SprintPlayerSpeed;
            } else if (gamepadState.justReleased(GamepadCode::LEFT_SHOULDER) || gamepadState.justReleased(
                           GamepadCode::RIGHT_SHOULDER)) {
                GlobalGameState->player.speed = BasePlayerSpeed;
            }

            Vector2<float> leftStick = gamepadState.leftStick.end;
            Vector2<float> direction = leftStick;
            direction.y *= -1;
            GlobalGameState->player.direction = direction.normalize();

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateController(controllerIdx, leftTrigger, rightTrigger);
        }
    }

    SLURP_LOAD_AUDIO(loadAudio) {}

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        drawColorPaletteSwatch(graphicsBuffer, {0, 0}, 25);

        GlobalUpdateRenderPipeline->process(graphicsBuffer, dt);

#if DEBUG
        if (GlobalRecordingState->isRecording) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {graphicsBuffer.widthPixels, graphicsBuffer.heightPixels},
                10,
                0x00FF0000
            );
        } else if (GlobalRecordingState->isPlayingBack) {
            render::drawRectBorder(
                graphicsBuffer,
                {0, 0},
                {graphicsBuffer.widthPixels, graphicsBuffer.heightPixels},
                10,
                0x0000FF00
            );
        }
#endif
    }
}
