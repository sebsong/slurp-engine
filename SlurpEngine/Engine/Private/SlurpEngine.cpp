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

    static const std::string ColorPaletteHexFileName = "slso8.hex";
    // static const std::string ColorPaletteHexFileName = "dead-weight-8.hex";
    // static const std::string ColorPaletteHexFileName = "lava-gb.hex";
    static constexpr float LowScrollSpeed = 1;
    static constexpr float HighScrollSpeed = 5;
    static constexpr float BaseFrequencyHz = 360;
    static constexpr float DeltaFrequencyHz = 220;

    static const Vector2<int> PlayerStartPos = {640, 360};
    static constexpr int PlayerSizePixels = 10;
    static constexpr int PlayerSpeed = 5;

    static void loadSineWave(AudioBuffer buffer)
    {
        float sineWavePeriod = buffer.samplesPerSec / GlobalGameState->frequencyHz;

        int16_t* subSamples = reinterpret_cast<int16_t*>(buffer.samples);
        for (int regionSampleIndex = 0; regionSampleIndex < buffer.samplesToWrite; regionSampleIndex++)
        {
            int16_t subSampleData = static_cast<int16_t>(sinf(GlobalGameState->tWave) * GlobalVolume);
            // *buffer.samples++ = (subSampleData << 16) | subSampleData;
            *subSamples++ = subSampleData;
            *subSamples++ = subSampleData;

            GlobalGameState->tWave += 2 * Pi / sineWavePeriod;
            GlobalGameState->tWave = std::fmod(GlobalGameState->tWave, 2 * Pi);
        }
    }

    static void loadSquareWave(AudioBuffer buffer)
    {
        float squareWavePeriod = buffer.samplesPerSec / GlobalGameState->frequencyHz;

        int16_t* subSamples = reinterpret_cast<int16_t*>(buffer.samples);
        for (int regionSampleIndex = 0; regionSampleIndex < buffer.samplesToWrite; regionSampleIndex++)
        {
            int16_t square = ((int)GlobalGameState->tWave % 2 == 0) ? 1 : -1;
            int16_t sampleData = static_cast<int16_t>(square * GlobalVolume / 4); // artificially lower volume
            *subSamples++ = sampleData;
            *subSamples++ = sampleData;

            GlobalGameState->tWave += 1 / (squareWavePeriod / 2.f);
        }
    }

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
        std::ifstream file(paletteHexFileName);

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
        GlobalGameState->scrollSpeed = LowScrollSpeed;
        GlobalGameState->frequencyHz = BaseFrequencyHz;
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
            GlobalGameState->graphicsDY -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerPos.y -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::A))
        {
            GlobalGameState->graphicsDX -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerPos.x -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::S))
        {
            GlobalGameState->graphicsDY += GlobalGameState->scrollSpeed;
            GlobalGameState->playerPos.y += PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::D))
        {
            GlobalGameState->graphicsDX += GlobalGameState->scrollSpeed;
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

        DigitalInputState inputState;
        if (keyboardState.getState(KeyboardCode::SPACE, inputState))
        {
            if (inputState.isDown)
            {
                GlobalGameState->scrollSpeed = 5;
            }
            else if (!inputState.isDown)
            {
                GlobalGameState->scrollSpeed = 1;
            }
        }

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

            if (gamepadState.isDown(GamepadCode::LEFT_SHOULDER) || gamepadState.isDown(GamepadCode::RIGHT_SHOULDER))
            {
                GlobalGameState->scrollSpeed = HighScrollSpeed;
            }
            else
            {
                GlobalGameState->scrollSpeed = LowScrollSpeed;
            }

            Vector2<float> leftStick = gamepadState.leftStick.end;
            GlobalGameState->graphicsDX += leftStick.x * GlobalGameState->scrollSpeed;
            GlobalGameState->graphicsDY -= leftStick.y * GlobalGameState->scrollSpeed;
            GlobalGameState->playerPos += static_cast<Vector2<int>>(leftStick) * PlayerSpeed;

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateController(controllerIdx, leftTrigger, rightTrigger);

            GlobalGameState->frequencyHz = BaseFrequencyHz + leftStick.x * DeltaFrequencyHz;
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
