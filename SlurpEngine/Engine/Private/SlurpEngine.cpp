#include <SlurpEngine.hpp>
#include <iostream>

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

    static constexpr float LowScrollSpeed = 1;
    static constexpr float HighScrollSpeed = 5;
    static constexpr float BaseFrequencyHz = 360;
    static constexpr float DeltaFrequencyHz = 220;

    static constexpr float PlayerStartX = 640;
    static constexpr float PlayerStartY = 360;
    static constexpr float PlayerSizePixels = 10;
    static constexpr float PlayerSpeed = 5;

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

    static void drawColorfulTriangles(const GraphicsBuffer buffer)
    {
        byte* bitmapBytes = static_cast<byte*>(buffer.memory);
        for (int y = 0; y < buffer.heightPixels; y++)
        {
            uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
            for (int x = 0; x < buffer.widthPixels; x++)
            {
                uint8_t r = static_cast<uint8_t>(y + GlobalGameState->graphicsDY);
                uint8_t g = static_cast<uint8_t>((x + GlobalGameState->graphicsDX) - (y + GlobalGameState->graphicsDY));
                uint8_t b = static_cast<uint8_t>(x + GlobalGameState->graphicsDX);

                uint32_t pixel = (r << 16) | (g << 8) | b;
                *rowPixels++ = pixel;
            }

            bitmapBytes += buffer.pitchBytes;
        }
    }

    static void drawBox(const GraphicsBuffer buffer, float xPos, float yPos, uint32_t color)
    {
        uint32_t* pixels = reinterpret_cast<uint32_t*>(buffer.memory);
        for (int dY = 0; dY < PlayerSizePixels; dY++)
        {
            for (int dX = 0; dX < PlayerSizePixels; dX++)
            {
                int x = static_cast<int>(xPos) + dX;
                int y = static_cast<int>(yPos) + dY;
                if (x < 0 || x >= buffer.widthPixels || y < 0 || y >= buffer.heightPixels)
                {
                    continue;
                }
                *(pixels + x + (y * buffer.widthPixels)) = color;
            }
        }
    }

    static void drawBorder(const GraphicsBuffer buffer, uint8_t borderWidth, uint32_t color)
    {
        uint32_t* pixels = reinterpret_cast<uint32_t*>(buffer.memory);
        for (int y = 0; y < buffer.heightPixels; y++)
        {
            int x = 0;
            while (x < buffer.widthPixels)
            {
                uint32_t* pixel = pixels + (y * buffer.widthPixels) + x;
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
            GlobalGameState->playerX = PlayerStartX;
            GlobalGameState->playerY = PlayerStartY;
        }
        GlobalGameState->isInitialized = true;

#if DEBUG
        assert(sizeof(RecordingState) <= gameMemory->transientMemory.sizeBytes);
        GlobalRecordingState = static_cast<RecordingState*>(gameMemory->transientMemory.memory);
#endif
    }

    SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(handleMouseAndKeyboardInput)
    {
        GlobalGameState->mouseX = mouseState.x;
        GlobalGameState->mouseY = mouseState.y;

        if (keyboardState.isDown(KeyboardCode::ALT) && keyboardState.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.shutdown();
        }

        if (keyboardState.isDown(KeyboardCode::W))
        {
            GlobalGameState->graphicsDY -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerY -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::A))
        {
            GlobalGameState->graphicsDX -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerX -= PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::S))
        {
            GlobalGameState->graphicsDY += GlobalGameState->scrollSpeed;
            GlobalGameState->playerY += PlayerSpeed;
        }
        if (keyboardState.isDown(KeyboardCode::D))
        {
            GlobalGameState->graphicsDX += GlobalGameState->scrollSpeed;
            GlobalGameState->playerX += PlayerSpeed;
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

            XYCoord leftStick = gamepadState.leftStick.endXY;
            GlobalGameState->graphicsDX += leftStick.x * GlobalGameState->scrollSpeed;
            GlobalGameState->graphicsDY -= leftStick.y * GlobalGameState->scrollSpeed;
            GlobalGameState->playerX += leftStick.x * PlayerSpeed;
            GlobalGameState->playerY -= leftStick.y * PlayerSpeed;

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.vibrateController(controllerIdx, leftTrigger, rightTrigger);

            GlobalGameState->frequencyHz = BaseFrequencyHz + leftStick.x * DeltaFrequencyHz;
        }
    }

    SLURP_LOAD_AUDIO(loadAudio)
    {
        loadSineWave(buffer);
        // loadSquareWave(buffer);
    }

    SLURP_UPDATE_AND_RENDER(updateAndRender)
    {
        drawColorfulTriangles(buffer);
        drawBox(buffer, GlobalGameState->playerX, GlobalGameState->playerY, 0x00000000);
        drawBox(buffer, GlobalGameState->mouseX, GlobalGameState->mouseY, 0x00CF9FFF);
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
