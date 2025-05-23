﻿#include <SlurpEngine.hpp>
#include <Debug.hpp>

// Single translation unit, unity build
#include <Update.cpp>
#include <Render.cpp>

typedef unsigned char byte;

namespace slurp
{
    static platform::PlatformDll GlobalPlatformDll;
    static GameState* GlobalGameState;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static constexpr int MouseCursorSizePixels = 10;
    static constexpr ColorPaletteIdx MouseCursorColorPalletIdx = 1;

    static const Vector2<int> PlayerStartPos = {640, 360};
    static constexpr int BasePlayerSizePixels = 20;
    static constexpr ColorPaletteIdx PlayerColorPalletIdx = 2;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;

    static const Vector2<int> EnemyStartPos = {400, 200};
    static const Vector2<int> EnemyPosOffset = {100, 0};
    static constexpr int BaseEnemySizePixels = 20;
    static constexpr ColorPaletteIdx EnemyColorPalletIdx = 4;
    static constexpr int BaseEnemySpeed = 200;

    static constexpr int ProjectileSizePixels = 15;
    static constexpr ColorPaletteIdx ProjectileColorPalletIdx = 1;
    static constexpr int BaseProjectileSpeed = 1000;

    static constexpr uint8_t BaseTileSize = 40;
    static constexpr std::array<std::array<ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> BaseTileMap =
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
        const GraphicsBuffer& buffer,
        Vector2<int> mousePosition,
        int size,
        ColorPaletteIdx colorPaletteIdx
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
        const GraphicsBuffer& buffer,
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

    static void drawColorPaletteSwatch(GraphicsBuffer buffer, Vector2<int> point, int size)
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
    }

    SLURP_INIT(init)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(GameState) <= gameMemory->permanentMemory.sizeBytes);
        GlobalGameState = static_cast<GameState*>(gameMemory->permanentMemory.memory);
        GlobalGameState->colorPalette = DEBUG_loadColorPalette(ColorPaletteHexFileName);
        if (!GlobalGameState->isInitialized)
        {
            GlobalGameState->tilemap.map = BaseTileMap;
            GlobalGameState->tilemap.tileSize = BaseTileSize;

            GlobalGameState->mouseCursor.size = MouseCursorSizePixels;
            GlobalGameState->mouseCursor.color = MouseCursorColorPalletIdx;

            GlobalGameState->player.size = BasePlayerSizePixels;
            GlobalGameState->player.color = PlayerColorPalletIdx;
            GlobalGameState->player.speed = BasePlayerSpeed;
            GlobalGameState->player.position = PlayerStartPos;
            setSquareCollisionPoints(GlobalGameState->player);

            for (int i = 0; i < NUM_ENEMIES; i++)
            {
                GlobalGameState->enemies[i].size = BaseEnemySizePixels;
                GlobalGameState->enemies[i].speed = BaseEnemySpeed;
                GlobalGameState->enemies[i].color = EnemyColorPalletIdx;
                GlobalGameState->enemies[i].position = EnemyStartPos + (EnemyPosOffset * i);
                setSquareCollisionPoints(GlobalGameState->enemies[i]);
            }

            for (Entity& projectile : GlobalGameState->projectiles)
            {
                projectile.enabled = false;
                projectile.size = ProjectileSizePixels;
                projectile.color = ProjectileColorPalletIdx;
                projectile.speed = BaseProjectileSpeed;
                setSquareCollisionPoints(projectile);
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

        if (mouseState.justPressed(MouseCode::Left))
        {
            Entity& projectile = GlobalGameState->projectiles[GlobalGameState->projectileIdx];
            projectile.enabled = true;
            projectile.position = GlobalGameState->player.position;
            projectile.direction =
                static_cast<Vector2<float>>(mouseState.position - GlobalGameState->player.position).normalize();
            GlobalGameState->projectileIdx++;
            if (GlobalGameState->projectileIdx >= PROJECTILE_POOL_SIZE)
            {
                GlobalGameState->projectileIdx = 0;
            }
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
        GlobalGameState->player.direction = direction.normalize();


        if (keyboardState.justPressed(KeyboardCode::SPACE))
        {
            GlobalGameState->player.speed = SprintPlayerSpeed;
        }
        else if (keyboardState.justReleased(KeyboardCode::SPACE))
        {
            GlobalGameState->player.speed = BasePlayerSpeed;
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
                GlobalGameState->player.speed = SprintPlayerSpeed;
            }
            else if (gamepadState.justReleased(GamepadCode::LEFT_SHOULDER) || gamepadState.justReleased(
                GamepadCode::RIGHT_SHOULDER))
            {
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

    SLURP_LOAD_AUDIO(loadAudio)
    {
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender)
    {
        // Update
        updatePosition(GlobalGameState->player, GlobalGameState->tilemap, dt);
        for (Entity& projectile : GlobalGameState->projectiles)
        {
            if (projectile.enabled)
            {
                updatePosition(projectile, GlobalGameState->tilemap, dt);
            }
        }

        // Render
        drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, buffer.heightPixels},
            7,
            GlobalGameState->colorPalette
        );

        drawTilemap(buffer, GlobalGameState->tilemap);
        drawColorPaletteSwatch(buffer, {0, 0}, 50);

        for (const Entity& enemy : GlobalGameState->enemies)
        {
            drawEntity(
                buffer,
                enemy,
                GlobalGameState->colorPalette
            );
        }

        for (Entity& projectile : GlobalGameState->projectiles)
        {
            if (projectile.enabled)
            {
                drawEntity(
                    buffer,
                    projectile,
                    GlobalGameState->colorPalette
                );
            }
        }

        drawEntity(
            buffer,
            GlobalGameState->player,
            GlobalGameState->colorPalette
        );

        drawEntity(
            buffer,
            GlobalGameState->mouseCursor,
            GlobalGameState->colorPalette
        );
#if DEBUG
        if (GlobalRecordingState->isRecording)
        {
            drawBorder(buffer, 10, 0x00FF0000);
        }
        else if (GlobalRecordingState->isPlayingBack)
        {
            drawBorder(buffer, 10, 0x0000FF00);
        }
#endif
    }
}
