#include <SlurpEngine.hpp>
#include <iostream>
#include <fstream>
#include <string>
typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr float GlobalVolume = 0.1f * 32000;

namespace slurp
{
    static platform::PlatformDll GlobalPlatformDll;
    static GameState* GlobalGameState;
#if DEBUG
    static RecordingState* GlobalRecordingState;
#endif

    static const std::string AssetsDirectory = "../assets/";
    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";

    static const Vector2<int> PlayerStartPos = {640, 360};
    static constexpr int BasePlayerSizePixels = 20;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;

    static constexpr uint8_t BaseTileSize = 40;
    static constexpr std::array<std::array<ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> BaseTileMap =
    {
        {
            {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 3, 2, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 3, 2, 1, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 3, 3, 3, 7, 7, 7, 7, 7, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 3, 3, 3, 1, 1, 1, 1, 1, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 3, 3, 3, 7, 7, 7, 7, 7, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6}},
            {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}},
        }
    };

    static void drawAtPoint(GraphicsBuffer buffer, Vector2<int> point, Pixel color)
    {
        *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = color;
    }

    static void _drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        Pixel color
    )
    {
        int minX = std::max(minPoint.x, 0);
        int maxX = std::min(maxPoint.x, buffer.widthPixels);
        int minY = std::max(minPoint.y, 0);
        int maxY = std::min(maxPoint.y, buffer.heightPixels);
        for (int y = minY; y < maxY; y++)
        {
            for (int x = minX; x < maxX; x++)
            {
                drawAtPoint(buffer, {x, y}, color);
            }
        }
    }

    static uint8_t round(float num)
    {
        return static_cast<uint8_t>(num + 0.5f);
    }

    static void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        float r,
        float g,
        float b
    )
    {
        uint8_t red = round(r * 255);
        uint8_t green = round(g * 255);
        uint8_t blue = round(b * 255);
        Pixel color = (red << 16) | (green << 8) | blue;
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    static void drawRect(
        const GraphicsBuffer& buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        ColorPaletteIdx colorPaletteIdx
    )
    {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        Pixel color = GlobalGameState->colorPalette.colors[colorPaletteIdx];
        _drawRect(buffer, minPoint, maxPoint, color);
    }

    static void drawSquare(
        const GraphicsBuffer& buffer,
        Vector2<int> point,
        int size,
        ColorPaletteIdx colorPaletteIdx
    )
    {
        drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            colorPaletteIdx
        );
    }

    static void drawPlayer(
        const GraphicsBuffer& buffer,
        Vector2<int> playerPosition,
        int size,
        ColorPaletteIdx colorPaletteIdx
    )
    {
        drawSquare(
            buffer,
            playerPosition,
            size,
            colorPaletteIdx
        );
    }

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
            colorPaletteIdx
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
                drawSquare(buffer, {x * tilemap.tileSize, y * tilemap.tileSize}, tilemap.tileSize, colorPaletteIdx);
            }
        }
    }

    static ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName)
    {
        ColorPalette palette = {};

        const std::string filePath = AssetsDirectory + paletteHexFileName;
        std::ifstream file(filePath);

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE)
        {
            Pixel color = std::stoi(line, nullptr, 16);
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
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
                i
            );
            position.x += size;
        }
    }

    static void drawBorder(const GraphicsBuffer& buffer, uint8_t borderThickness, uint32_t color)
    {
        _drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, borderThickness},
            color
        );
        _drawRect(
            buffer,
            {0, 0},
            {borderThickness, buffer.heightPixels},
            color
        );
        _drawRect(
            buffer,
            {buffer.widthPixels - borderThickness, 0},
            {buffer.widthPixels, buffer.heightPixels},
            color
        );
        _drawRect(
            buffer,
            {0, buffer.heightPixels - borderThickness},
            {buffer.widthPixels, buffer.heightPixels},
            color
        );
    }

    SLURP_INIT(init)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(GameState) <= gameMemory->permanentMemory.sizeBytes);
        GlobalGameState = static_cast<GameState*>(gameMemory->permanentMemory.memory);
        if (!GlobalGameState->isInitialized)
        {
            GlobalGameState->tilemap.map = BaseTileMap;
            GlobalGameState->tilemap.tileSize = BaseTileSize;

            GlobalGameState->player.position = PlayerStartPos;
            GlobalGameState->player.size = BasePlayerSizePixels;
            GlobalGameState->player.speed = BasePlayerSpeed;

            int sizeCoord = GlobalGameState->player.size - 1;
            GlobalGameState->player.relativeCollisionPoints[0] = {0, 0};
            GlobalGameState->player.relativeCollisionPoints[1] = {sizeCoord, 0};
            GlobalGameState->player.relativeCollisionPoints[2] = {0, sizeCoord};
            GlobalGameState->player.relativeCollisionPoints[3] = {sizeCoord, sizeCoord};
            GlobalGameState->isInitialized = true;
        }
        GlobalGameState->colorPalette = DEBUG_loadColorPalette(ColorPaletteHexFileName);

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
    }

    static int getAxisPositionUpdate(
        int currentAxisPosition,
        int axisPositionUpdate,
        ColorPaletteIdx newTilemapValue,
        uint8_t tileSize
    )
    {
        int newAxisPosition = currentAxisPosition + axisPositionUpdate;
        int currentAxisTilemapPosition = currentAxisPosition / tileSize;
        int newAxisTilemapPosition = newAxisPosition / tileSize;

        int updatedAxisPosition = currentAxisPosition;
        if (newTilemapValue == (COLOR_PALETTE_SIZE - 1))
        {
            updatedAxisPosition = newAxisPosition;
        }
        else
        {
            if (newAxisTilemapPosition > currentAxisTilemapPosition)
            {
                updatedAxisPosition = newAxisTilemapPosition * tileSize - 1;
            }
            else if (newAxisTilemapPosition < currentAxisTilemapPosition)
            {
                updatedAxisPosition = currentAxisTilemapPosition * tileSize;
            }
        }

        return updatedAxisPosition - currentAxisPosition;
    }

    static Vector2<int> getPositionUpdate(Vector2<int> currentPosition, Vector2<int> positionUpdate, Tilemap tilemap)
    {
        Vector2<int> newPosition = currentPosition + positionUpdate;
        Vector2<int> currentTilemapPosition = currentPosition / tilemap.tileSize;
        Vector2<int> newTilemapPosition = newPosition / tilemap.tileSize;
        if (
            newTilemapPosition.x >= 0 && newTilemapPosition.x < TILEMAP_WIDTH &&
            newTilemapPosition.y >= 0 && newTilemapPosition.y < TILEMAP_HEIGHT
        )
        {
            ColorPaletteIdx newXAxisTilemapValue = tilemap.map[currentTilemapPosition.y][newTilemapPosition.x];
            ColorPaletteIdx newYAxisTilemapValue = tilemap.map[newTilemapPosition.y][currentTilemapPosition.x];
            int xAxisPositionUpdate = getAxisPositionUpdate(
                currentPosition.x,
                positionUpdate.x,
                newXAxisTilemapValue,
                tilemap.tileSize
            );
            int yAxisPositionUpdate = getAxisPositionUpdate(
                currentPosition.y,
                positionUpdate.y,
                newYAxisTilemapValue,
                tilemap.tileSize
            );
            return {xAxisPositionUpdate, yAxisPositionUpdate};
        }
        return Vector2<int>::Zero;
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput)
    {
        GlobalGameState->mousePosition = mouseState.position;

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.shutdown();
        }

        Vector2<float> dPosition;
        if (keyboardState.isDown(KeyboardCode::W))
        {
            dPosition.y -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::A))
        {
            dPosition.x -= 1;
        }
        if (keyboardState.isDown(KeyboardCode::S))
        {
            dPosition.y += 1;
        }
        if (keyboardState.isDown(KeyboardCode::D))
        {
            dPosition.x += 1;
        }

        // handle collision check against tilemap
        // TODO: refactor these names and move to separate method
        Vector2<int> positionUpdate = (dPosition * GlobalGameState->player.speed * dt);
        Vector2<int> update = positionUpdate;
        for (Vector2<int> relativePoint : GlobalGameState->player.relativeCollisionPoints)
        {
            Vector2<int> point = GlobalGameState->player.position + relativePoint;
            Vector2<int> pointPositionUpdate = getPositionUpdate(point, positionUpdate, GlobalGameState->tilemap);
            if (std::abs(pointPositionUpdate.x) < std::abs(update.x))
            {
                update.x = pointPositionUpdate.x;
            }
            if (std::abs(pointPositionUpdate.y) < std::abs(update.y))
            {
                update.y = pointPositionUpdate.y;
            }
        }
        GlobalGameState->player.position += update;

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
            Vector2<float> dPosition = leftStick * GlobalGameState->player.speed * dt;
            dPosition.y *= -1;
            GlobalGameState->player.position += dPosition;

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
        drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, buffer.heightPixels},
            7
        );

        drawTilemap(buffer, GlobalGameState->tilemap);
        drawColorPaletteSwatch(buffer, {0, 0}, 50);

        drawPlayer(
            buffer,
            GlobalGameState->player.position,
            GlobalGameState->player.size,
            2
        );
        drawMouse(
            buffer,
            GlobalGameState->mousePosition,
            BasePlayerSizePixels,
            4
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
