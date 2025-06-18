#include "SlurpEngine.h"
#include "Random.h"
#include "Debug.h"

// Single translation unit, unity build
#include "Update.cpp"
#include "Collision.cpp"
#include "Render.cpp"
#include "UpdateRenderPipeline.cpp"
#include "Timer.cpp"

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

    static constexpr uint8_t BaseTileSize = 40;
    static constexpr std::array<std::array<render::ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> BaseTileMap =
    {
        {
            {{7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 3, 3, 3, 6, 6, 6, 6, 6, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 3, 3, 3, 6, 6, 6, 6, 6, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7}},
            {{7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7}},
        }
    };

    static void drawMouse(
        const render::GraphicsBuffer& buffer,
        Vector2<int> mousePosition,
        int size,
        render::ColorPaletteIdx colorPaletteIdx
    ) {
        Vector2<int> point = mousePosition - Vector2<int>::Unit * size / 2;
        drawSquare(
            buffer,
            point,
            size,
            colorPaletteIdx,
            GlobalGameState->colorPalette
        );
    }

    static void drawTilemap(
        const render::GraphicsBuffer& buffer,
        const Tilemap tilemap
    ) {
        for (int y = 0; y < TILEMAP_HEIGHT; y++) {
            for (int x = 0; x < TILEMAP_WIDTH; x++) {
                uint8_t colorPaletteIdx = tilemap.map[y][x];
                drawSquare(
                    buffer,
                    {x * tilemap.tileSize, y * tilemap.tileSize},
                    tilemap.tileSize,
                    colorPaletteIdx,
                    GlobalGameState->colorPalette
                );
            }
        }
    }

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

    static void setSquareCollisionPoints(Entity& entity) {
        int sizeCoord = entity.size - 1;
        entity.relativeCollisionPoints[0] = {0, 0};
        entity.relativeCollisionPoints[1] = {sizeCoord, 0};
        entity.relativeCollisionPoints[2] = {0, sizeCoord};
        entity.relativeCollisionPoints[3] = {sizeCoord, sizeCoord};

        for (Vector2<int>& collisionPoint: entity.relativeCollisionPoints) {
            collisionPoint -= entity.renderOffset;
        }

        entity.collisionSquare.radius = entity.size / 2;
        entity.collisionEnabled = true;
    }

    static void setRandomDirection(Entity& entity) {
        float randX = random::randomFloat(-1, 1);
        float randY = random::randomFloat(-1, 1);
        entity.direction = Vector2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay() {
        float minDelay = BaseEnemyDirectionChangeDelay - EnemyDirectionChangeDelayDelta;
        float maxDelay = BaseEnemyDirectionChangeDelay + EnemyDirectionChangeDelayDelta;
        return random::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(Entity& enemy) {
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

        // TODO: make walls
        // TODO: have a method for creating entities and registering them with an id

        new (&GlobalGameState->obstacle1) Entity();
        Entity& obstacle1 = GlobalGameState->obstacle1;
        obstacle1.name = "Obstacle1";
        obstacle1.enabled = true;
        obstacle1.collisionEnabled = true;
        obstacle1.isStatic = true;
        obstacle1.size = 150;
        obstacle1.color = 5;
        obstacle1.position = {200, 500};
        obstacle1.renderOffset = Vector2<int>::Unit * obstacle1.size / 2;
        setSquareCollisionPoints(obstacle1);
        GlobalUpdateRenderPipeline->push(obstacle1);

        // GlobalGameState->tilemap.map = BaseTileMap;
        // GlobalGameState->tilemap.tileSize = BaseTileSize;

        Entity& mouseCursor = GlobalGameState->mouseCursor;
        mouseCursor.name = "MouseCursor";
        mouseCursor.enabled = true;
        mouseCursor.size = MouseCursorSizePixels;
        mouseCursor.color = MouseCursorColorPalletIdx;
        mouseCursor.renderOffset = Vector2<int>::Unit * mouseCursor.size / 2;
        GlobalUpdateRenderPipeline->push(mouseCursor);

        Entity& playerEntity = GlobalGameState->player.entity;
        playerEntity.name = "Player";
        playerEntity.enabled = true;
        playerEntity.size = BasePlayerSizePixels;
        playerEntity.color = PlayerColorPalletIdx;
        playerEntity.speed = BasePlayerSpeed;
        playerEntity.position = PlayerStartPos;
        playerEntity.renderOffset = Vector2<int>::Unit * playerEntity.size /
                                      2;
        setSquareCollisionPoints(playerEntity);
        GlobalUpdateRenderPipeline->push(playerEntity);

        for (int i = 0; i < NUM_ENEMIES; i++) {
            Entity& enemy = GlobalGameState->enemies[i];
            new (&enemy) Entity();
            enemy.name = "enemy" + std::to_string(i);
            enemy.enabled = true;
            enemy.size = BaseEnemySizePixels;
            enemy.speed = BaseEnemySpeed;
            enemy.position = EnemyStartPos + (EnemyPosOffset * i);
            enemy.color = EnemyColorPalletIdx;
            enemy.renderOffset = Vector2<int>::Unit * enemy.size / 2;
            setSquareCollisionPoints(enemy);
            // startUpdateEnemyDirection(enemy); // TODO: re-enable this
            GlobalUpdateRenderPipeline->push(enemy);
        }

        for (int i = 0; i < PROJECTILE_POOL_SIZE; i++) {
            Entity& projectile = GlobalGameState->projectiles[i];
            new (&projectile) Entity();
            projectile.name = "projectile" + std::to_string(i);
            projectile.enabled = false;
            projectile.size = ProjectileSizePixels;
            projectile.renderOffset = Vector2<int>::Unit * projectile.size / 2;
            projectile.color = ProjectileColorPalletIdx;
            projectile.speed = BaseProjectileSpeed;
            setSquareCollisionPoints(projectile);
            GlobalUpdateRenderPipeline->push(projectile);
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
        player.entity.color = PlayerParryColorPalletIdx;
    }

    static void deactivateParry(Player& player) {
        player.isParryActive = false;
        player.entity.color = PlayerColorPalletIdx;
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput) {
        GlobalGameState->mouseCursor.position = mouseState.position;

        if (mouseState.justPressed(MouseCode::LeftClick)) {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.entity.position;
            projectile.direction =
                    static_cast<Vector2<float>>(mouseState.position - GlobalGameState->player.entity.position).
                    normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE) {
                GlobalGameState->projectileIdx = 0;
            }
        }

        if (mouseState.justPressed(MouseCode::RightClick)) {
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
        GlobalGameState->player.entity.direction = direction.normalize();


        if (keyboardState.justPressed(KeyboardCode::SPACE)) {
            GlobalGameState->player.entity.speed = SprintPlayerSpeed;
        } else if (keyboardState.justReleased(KeyboardCode::SPACE)) {
            GlobalGameState->player.entity.speed = BasePlayerSpeed;
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
                GlobalGameState->player.entity.speed = SprintPlayerSpeed;
            } else if (gamepadState.justReleased(GamepadCode::LEFT_SHOULDER) || gamepadState.justReleased(
                           GamepadCode::RIGHT_SHOULDER)) {
                GlobalGameState->player.entity.speed = BasePlayerSpeed;
            }

            Vector2<float> leftStick = gamepadState.leftStick.end;
            Vector2<float> direction = leftStick;
            direction.y *= -1;
            GlobalGameState->player.entity.direction = direction.normalize();

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateController(controllerIdx, leftTrigger, rightTrigger);
        }
    }

    SLURP_LOAD_AUDIO(loadAudio) {}

    static const Entity& findClosest(const Vector2<int>& position, const Entity* entities, int numEntities) {
        assert(numEntities > 0);

        int minIdx = 0;
        float minDistance = position.distanceTo(entities[0].position);

        for (int i = 1; i < numEntities; i++) {
            const Entity& entity = entities[i];
            float distance = position.distanceTo(entity.position);
            if (distance < minDistance) {
                minIdx = i;
                minDistance = distance;
            }
        }

        return entities[minIdx];
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender) {
        timer::tick(dt);

        // // Draw background
        render::drawRect(
            graphicsBuffer,
            {0, 0},
            {graphicsBuffer.widthPixels, graphicsBuffer.heightPixels},
            7,
            GlobalGameState->colorPalette
        );
        drawColorPaletteSwatch(graphicsBuffer, {0, 0}, 25);

        GlobalUpdateRenderPipeline->process(graphicsBuffer, dt);

        for (const Entity& projectile: GlobalGameState->projectiles) {
            if (projectile.enabled) {
                const Entity& closestEnemy = findClosest(projectile.position, GlobalGameState->enemies, NUM_ENEMIES);
                render::drawLine(
                    graphicsBuffer,
                    projectile.position,
                    closestEnemy.position,
                    2,
                    0,
                    GlobalGameState->colorPalette
                );
            }
        }

        render::drawLine(
            graphicsBuffer,
            GlobalGameState->mouseCursor.position,
            GlobalGameState->player.entity.position,
            2,
            5,
            GlobalGameState->colorPalette
        );

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
