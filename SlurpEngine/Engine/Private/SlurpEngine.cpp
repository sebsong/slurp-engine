#include <SlurpEngine.hpp>
#include <Debug.hpp>

// Single translation unit, unity build
#include <Update.cpp>
#include <Render.cpp>
#include <UpdateRenderPipeline.cpp>
#include <Timer.cpp>

#include <random>

typedef unsigned char byte;

namespace slurp
{
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
    static constexpr render::ColorPaletteIdx PlayerColorPalletIdx = 2;
    static constexpr render::ColorPaletteIdx PlayerParryColorPalletIdx = 1;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;

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
    )
    {
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
    )
    {
        for (int y = 0; y < TILEMAP_HEIGHT; y++)
        {
            for (int x = 0; x < TILEMAP_WIDTH; x++)
            {
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

    static void drawColorPaletteSwatch(render::GraphicsBuffer buffer, Vector2<int> point, int size)
    {
        Vector2<int> position = point;
        for (uint8_t i = 0; i < COLOR_PALETTE_SIZE; i++)
        {
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

    static void setSquareCollisionPoints(Entity& entity)
    {
        int sizeCoord = entity.size - 1;
        entity.relativeCollisionPoints[0] = {0, 0};
        entity.relativeCollisionPoints[1] = {sizeCoord, 0};
        entity.relativeCollisionPoints[2] = {0, sizeCoord};
        entity.relativeCollisionPoints[3] = {sizeCoord, sizeCoord};

        for (Vector2<int>& collisionPoint : entity.relativeCollisionPoints)
        {
            collisionPoint -= entity.positionOffset;
        }
    }

    static void setRandomDirection(Entity& entity)
    {
        float randX = math::randomFloat(-1, 1);
        float randY = math::randomFloat(-1, 1);
        entity.direction = Vector2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay()
    {
        float minDelay = BaseEnemyDirectionChangeDelay - EnemyDirectionChangeDelayDelta;
        float maxDelay = BaseEnemyDirectionChangeDelay + EnemyDirectionChangeDelayDelta;
        return math::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(Entity& enemy)
    {
        setRandomDirection(enemy);
        timer::delay(getRandomDirectionChangeDelay(), [&]
        {
            startUpdateEnemyDirection(enemy);
        });
    }

    SLURP_INIT(init)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(MemorySections) <= gameMemory->permanentMemory.sizeBytes);
        MemorySections* sections = static_cast<MemorySections*>(gameMemory->permanentMemory.memory);
        GlobalGameState = &sections->gameState;
        GlobalUpdateRenderPipeline = &sections->updateRenderPipeline;

        GlobalGameState->colorPalette = render::DEBUG_loadColorPalette(ColorPaletteHexFileName);
        if (!GlobalGameState->isInitialized)
        {
            GlobalGameState->tilemap.map = BaseTileMap;
            GlobalGameState->tilemap.tileSize = BaseTileSize;

            GlobalGameState->mouseCursor.enabled = true;
            GlobalGameState->mouseCursor.size = MouseCursorSizePixels;
            GlobalGameState->mouseCursor.color = MouseCursorColorPalletIdx;
            GlobalGameState->mouseCursor.positionOffset = Vector2<int>::Unit * GlobalGameState->mouseCursor.size / 2;
            GlobalUpdateRenderPipeline->push(GlobalGameState->mouseCursor);

            GlobalGameState->player.entity.enabled = true;
            GlobalGameState->player.entity.size = BasePlayerSizePixels;
            GlobalGameState->player.entity.color = PlayerColorPalletIdx;
            GlobalGameState->player.entity.speed = BasePlayerSpeed;
            GlobalGameState->player.entity.position = PlayerStartPos;
            GlobalGameState->player.entity.positionOffset = Vector2<int>::Unit * GlobalGameState->player.entity.size /
                2;
            setSquareCollisionPoints(GlobalGameState->player.entity);
            GlobalUpdateRenderPipeline->push(GlobalGameState->player.entity);

            for (int i = 0; i < NUM_ENEMIES; i++)
            {
                Entity& enemy = GlobalGameState->enemies[i];
                enemy.enabled = true;
                enemy.size = BaseEnemySizePixels;
                enemy.speed = BaseEnemySpeed;
                enemy.color = EnemyColorPalletIdx;
                enemy.position = EnemyStartPos + (EnemyPosOffset * i);
                enemy.positionOffset = Vector2<int>::Unit * enemy.size / 2;
                setSquareCollisionPoints(enemy);
                startUpdateEnemyDirection(enemy);
                GlobalUpdateRenderPipeline->push(enemy);
            }

            for (Entity& projectile : GlobalGameState->projectiles)
            {
                projectile.enabled = false;
                projectile.size = ProjectileSizePixels;
                projectile.positionOffset = {.5, .5};
                projectile.color = ProjectileColorPalletIdx;
                projectile.speed = BaseProjectileSpeed;
                setSquareCollisionPoints(projectile);
                GlobalUpdateRenderPipeline->push(projectile);
            }

            GlobalGameState->isInitialized = true;
        }

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput)
    {
        GlobalGameState->mouseCursor.position = mouseState.position;

        if (mouseState.justPressed(MouseCode::LeftClick))
        {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.entity.position;
            projectile.direction =
                static_cast<Vector2<float>>(mouseState.position - GlobalGameState->player.entity.position).normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE)
            {
                GlobalGameState->projectileIdx = 0;
            }
        }

        if (mouseState.justPressed(MouseCode::RightClick))
        {
            GlobalGameState->player.isParryActive = !GlobalGameState->player.isParryActive;
            render::ColorPaletteIdx newColor;
            if (GlobalGameState->player.isParryActive)
            {
                newColor = PlayerParryColorPalletIdx;
            }
            else
            {
                newColor = PlayerColorPalletIdx;
            }
            GlobalGameState->player.entity.color = newColor;
        }

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.shutdown();
        }

        Vector2<float> direction;
        if (keyboardState.isDown(KeyboardCode::W))
        {
            direction.y -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::A))
        {
            direction.x -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::S))
        {
            direction.y += 1;
        }
        if (keyboardState.isDown(KeyboardCode::D))
        {
            direction.x += 1;
        }
        GlobalGameState->player.entity.direction = direction.normalize();


        if (keyboardState.justPressed(KeyboardCode::SPACE))
        {
            GlobalGameState->player.entity.speed = SprintPlayerSpeed;
        }
        else if (keyboardState.justReleased(KeyboardCode::SPACE))
        {
            GlobalGameState->player.entity.speed = BasePlayerSpeed;
        }

#if DEBUG
        if (keyboardState.justPressed(KeyboardCode::P))
        {
            GlobalPlatformDll.DEBUG_togglePause();
        }
        if (keyboardState.justPressed(KeyboardCode::R) && !GlobalRecordingState->isPlayingBack)
        {
            if (!GlobalRecordingState->isRecording)
            {
                GlobalRecordingState->isRecording = true;
                GlobalPlatformDll.DEBUG_beginRecording();
            }
            else
            {
                GlobalPlatformDll.DEBUG_endRecording();
                GlobalRecordingState->isRecording = false;
            }
        }
        if (keyboardState.justPressed(KeyboardCode::T))
        {
            GlobalRecordingState->isPlayingBack = true;
            auto onPlaybackEnd = []() -> void { GlobalRecordingState->isPlayingBack = false; };
            GlobalPlatformDll.DEBUG_beginPlayback(onPlaybackEnd);
        }
#endif

        if (keyboardState.isDown(KeyboardCode::ESC))
        {
            GlobalPlatformDll.shutdown();
        }
    }

    SLURP_HANDLE_GAMEPAD_INPUT(handleGamepadInput)
    {
        for (int controllerIdx = 0; controllerIdx < MAX_NUM_CONTROLLERS; controllerIdx++)
        {
            GamepadState gamepadState = controllerStates[controllerIdx];
            if (!gamepadState.isConnected)
            {
                continue;
            }

            if (gamepadState.isDown(GamepadCode::START) || gamepadState.isDown(GamepadCode::B))
            {
                GlobalPlatformDll.shutdown();
            }

            if (gamepadState.justPressed(GamepadCode::LEFT_SHOULDER) || gamepadState.justPressed(
                GamepadCode::RIGHT_SHOULDER))
            {
                GlobalGameState->player.entity.speed = SprintPlayerSpeed;
            }
            else if (gamepadState.justReleased(GamepadCode::LEFT_SHOULDER) || gamepadState.justReleased(
                GamepadCode::RIGHT_SHOULDER))
            {
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

    SLURP_LOAD_AUDIO(loadAudio)
    {
    }

    static const Entity& findClosest(const Vector2<int>& position, const Entity* entities, int numEntities)
    {
        assert(numEntities > 0);

        int minIdx = 0;
        float minDistance = position.distanceTo(entities[0].position);

        for (int i = 1; i < numEntities; i++)
        {
            const Entity& entity = entities[i];
            float distance = position.distanceTo(entity.position);
            if (distance < minDistance)
            {
                minIdx = i;
                minDistance = distance;
            }
        }

        return entities[minIdx];
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender)
    {
        timer::tick(dt);

        // Draw background
        render::drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, buffer.heightPixels},
            7,
            GlobalGameState->colorPalette
        );
        drawTilemap(buffer, GlobalGameState->tilemap);
        drawColorPaletteSwatch(buffer, {0, 0}, 50);

        GlobalUpdateRenderPipeline->process(
            GlobalGameState->tilemap,
            dt,
            buffer,
            GlobalGameState->colorPalette
        );
        
        for (const Entity& projectile : GlobalGameState->projectiles)
        {
            if (projectile.enabled)
            {
                const Entity& closestEnemy = findClosest(projectile.position, GlobalGameState->enemies, NUM_ENEMIES);
                render::drawLine(
                    buffer,
                    projectile.position,
                    closestEnemy.position,
                    2,
                    0,
                    GlobalGameState->colorPalette
                );
            }
        }

        render::drawLine(
            buffer,
            GlobalGameState->mouseCursor.position,
            GlobalGameState->player.entity.position,
            2,
            5,
            GlobalGameState->colorPalette
        );
#if DEBUG
        if (GlobalRecordingState->isRecording)
        {
            render::drawBorder(buffer, 10, 0x00FF0000);
        }
        else if (GlobalRecordingState->isPlayingBack)
        {
            render::drawBorder(buffer, 10, 0x0000FF00);
        }
#endif
    }
}
