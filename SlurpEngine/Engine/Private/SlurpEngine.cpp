#include <SlurpEngine.hpp>
#include <iostream>

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr float GlobalVolume = 0.1f * 32000;

namespace slurp
{
    static platform::PlatformDll GlobalPlatformDll;
    static RecordingState* GlobalRecordingState;
    static GameState* GlobalGameState;

    static constexpr float LowScrollSpeed = 1;
    static constexpr float HighScrollSpeed = 5;
    static constexpr float BaseFrequencyHz = 360;
    static constexpr float DeltaFrequencyHz = 220;

    static constexpr float PlayerStartX = 640;
    static constexpr float PlayerStartY = 360;
    static constexpr float PlayerSizePixels = 10;
    static constexpr float PlayerSpeedX = 20;
    static constexpr float PlayerSpeedY = 5;

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

    static void drawPlayer(const GraphicsBuffer buffer, uint32_t color)
    {
        int xOffset = static_cast<int>(GlobalGameState->playerX);
        int yOffset = static_cast<int>(GlobalGameState->playerY) * buffer.pitchBytes;
        byte* startBitmapBytes = static_cast<byte*>(buffer.memory);
        byte* bitmapBytes = startBitmapBytes + xOffset + yOffset;
        size_t maxBitmapSize = buffer.widthPixels * buffer.heightPixels * sizeof(uint32_t);
        for (int y = 0; y < PlayerSizePixels; y++)
        {
            uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
            for (int x = 0; x < PlayerSizePixels; x++)
            {
                size_t bitmapOffset = bitmapBytes - startBitmapBytes;
                if (bitmapOffset < 0 || bitmapOffset >= maxBitmapSize)
                {
                    continue;
                }
                *rowPixels++ = color;
            }

            bitmapBytes += buffer.pitchBytes;
        }
    }

    SLURP_INIT(init)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(SlurpStates) <= gameMemory->permanentMemory.sizeBytes)
        
        SlurpStates* states = static_cast<SlurpStates*>(gameMemory->permanentMemory.memory);
        GlobalRecordingState = &states->recordingState;
        GlobalGameState = &states->gameState;
        
        GlobalGameState->scrollSpeed = LowScrollSpeed;
        GlobalGameState->frequencyHz = BaseFrequencyHz;
        GlobalGameState->playerX = PlayerStartX;
        GlobalGameState->playerY = PlayerStartY;
    }

    SLURP_HANDLE_KEYBOARD_INPUT(handleKeyboardInput)
    {
        if (state.isDown(KeyboardCode::ALT) && state.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.shutdown();
        }

        if (state.isDown(KeyboardCode::W))
        {
            GlobalGameState->graphicsDY -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerY -= PlayerSpeedY;
        }
        if (state.isDown(KeyboardCode::A))
        {
            GlobalGameState->graphicsDX -= GlobalGameState->scrollSpeed;
            GlobalGameState->playerX -= PlayerSpeedX;
        }
        if (state.isDown(KeyboardCode::S))
        {
            GlobalGameState->graphicsDY += GlobalGameState->scrollSpeed;
            GlobalGameState->playerY += PlayerSpeedY;
        }
        if (state.isDown(KeyboardCode::D))
        {
            GlobalGameState->graphicsDX += GlobalGameState->scrollSpeed;
            GlobalGameState->playerX += PlayerSpeedX;
        }
#if DEBUG
        if (state.justPressed(KeyboardCode::P))
        {
            GlobalPlatformDll.DEBUG_togglePause();
        }
        if (state.justPressed(KeyboardCode::R))
        {
            if (!GlobalRecordingState->isRecording)
            {
                std::cout << "begin recording" << std::endl;
                GlobalPlatformDll.DEBUG_beginRecording();
                GlobalRecordingState->isRecording = true;
            }
            else
            {
                std::cout << "end recording" << std::endl;
                GlobalPlatformDll.DEBUG_endRecording();
                GlobalRecordingState->isRecording = false;
            }
        }
        if (state.justPressed(KeyboardCode::T))
        {
            if (!GlobalRecordingState->isPlayingBack)
            {
                GlobalPlatformDll.DEBUG_beginPlayback();
                GlobalRecordingState->isPlayingBack = true;
            }
            else
            {
                GlobalPlatformDll.DEBUG_endPlayback();
                GlobalRecordingState->isPlayingBack = false;
            }
        }
#endif

        DigitalInputState inputState;
        if (state.getState(KeyboardCode::SPACE, inputState))
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

        if (state.isDown(KeyboardCode::ESC))
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
            GlobalGameState->playerX += leftStick.x * PlayerSpeedX;
            GlobalGameState->playerY -= leftStick.y * PlayerSpeedY;

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

    SLURP_RENDER_GRAPHICS(renderGraphics)
    {
        drawColorfulTriangles(buffer);
        drawPlayer(buffer, 0x00000000);
    }

    SLURP_UPDATE(update)
    {
        std::cout << "PLAYER: " << GlobalGameState->playerX << ":" << GlobalGameState->playerY << std::endl;
        std::cout << "GRAPHICS: " << GlobalGameState->graphicsDX << ":" << GlobalGameState->graphicsDY << std::endl;
    }
}
