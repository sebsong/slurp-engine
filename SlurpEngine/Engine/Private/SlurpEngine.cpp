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
    static constexpr int PlayerSizePixels = 10;
    static constexpr int PlayerSpeed = 5;

    static void drawAtPoint(GraphicsBuffer buffer, Vector2<int> point, uint8_t colorPaletteIdx)
    {
        assert(colorPaletteIdx < COLOR_PALETTE_SIZE);
        *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = GlobalGameState->colorPalette.colors[
            colorPaletteIdx];
    }

    static void drawRect(
        const GraphicsBuffer buffer,
        Vector2<int> minPoint,
        Vector2<int> maxPoint,
        uint8_t colorPaletteIdx
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
                drawAtPoint(buffer, {x, y}, colorPaletteIdx);
            }
        }
    }

    static void drawSquare(
        const GraphicsBuffer buffer,
        Vector2<int> point,
        int size,
        uint8_t colorPaletteIdx
    )
    {
        drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            colorPaletteIdx
        );
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

    static void drawBorder(const GraphicsBuffer buffer, uint8_t borderWidth, uint32_t color)
    {
        for (int y = 0; y < buffer.heightPixels; y++)
        {
            int x = 0;
            while (x < buffer.widthPixels)
            {
                uint32_t* pixel = buffer.pixelMap + (y * buffer.widthPixels) + x;
                if ((y < borderWidth || y > buffer.heightPixels - borderWidth) ||
                    (x < borderWidth || x > buffer.widthPixels - borderWidth))
                {
                    *pixel = color;
                }
                else if (x > borderWidth && x < buffer.widthPixels - borderWidth)
                {
                    x = buffer.widthPixels - borderWidth;
                    continue;
                }
                x++;
            }
        }
    }

    SLURP_INIT(init)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(GameState) <= gameMemory->permanentMemory.sizeBytes);
        GlobalGameState = static_cast<GameState*>(gameMemory->permanentMemory.memory);
        if (!GlobalGameState->isInitialized)
        {
            GlobalGameState->playerPos = PlayerStartPos;
        }
        GlobalGameState->isInitialized = true;
        GlobalGameState->colorPalette = DEBUG_loadColorPalette(ColorPaletteHexFileName);

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput)
    {
        GlobalGameState->mousePos = mouseState.position;

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.shutdown();
        }

        if (keyboardState.isDown(KeyboardCode::W))
        {
            GlobalGameState->playerPos.y -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::A))
        {
            GlobalGameState->playerPos.x -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::S))
        {
            GlobalGameState->playerPos.y += PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::D))
        {
            GlobalGameState->playerPos.x += PlayerSpeed;
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

            Vector2<float> leftStick = gamepadState.leftStick.end;
            Vector2<float> dPosition = leftStick * PlayerSpeed;
            dPosition.y *= -1;
            GlobalGameState->playerPos += dPosition;

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateController(controllerIdx, leftTrigger, rightTrigger);
        }
    }

    SLURP_LOAD_AUDIO(loadAudio)
    {
        // loadSineWave(buffer);
        // loadSquareWave(buffer);
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender)
    {
        // drawColorfulTriangles(buffer);
        drawRect(
            buffer,
            {0, 0},
            {buffer.widthPixels, buffer.heightPixels},
            7
        );
        drawColorPaletteSwatch(buffer, {0, 0}, 50);
        drawSquare(
            buffer,
            GlobalGameState->playerPos,
            PlayerSizePixels,
            2
        );
        drawSquare(
            buffer,
            GlobalGameState->mousePos,
            PlayerSizePixels,
            4
        );
#if DEBUG
        if (GlobalRecordingState->isRecording)
        {
            drawBorder(buffer, 5, 0x00FF0000);
        }
        else if (GlobalRecordingState->isPlayingBack)
        {
            drawBorder(buffer, 5, 0x0000FF00);
        }
#endif
    }
}
