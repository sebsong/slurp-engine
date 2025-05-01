#include <SlurpEngine.hpp>
#include <iostream>

#include "Platform.hpp"

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr float GlobalVolume = 0.1f * 32000;

namespace slurp
{
    static PlatformDll GlobalPlatformDll;
    static GameState* GlobalGameState;

    static constexpr float LowScrollSpeed = 1;
    static constexpr float HighScrollSpeed = 5;
    static constexpr float BaseFrequencyHz = 360;
    static constexpr float DeltaFrequencyHz = 220;

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

    void init(const PlatformDll platformDll, GameMemory* gameMemory)
    {
        GlobalPlatformDll = platformDll;

        assert(sizeof(GameState) <= gameMemory->permanentMemory.sizeBytes)
        GlobalGameState = static_cast<GameState*>(gameMemory->permanentMemory.memory);
        if (!gameMemory->isInitialized)
        {
            GlobalGameState->scrollSpeed = LowScrollSpeed;
            GlobalGameState->frequencyHz = BaseFrequencyHz;
            gameMemory->isInitialized = true;
        }
    }

    void handleKeyboardInput(KeyboardState state)
    {
        if (state.isDown(KeyboardCode::ALT) && state.isDown(KeyboardCode::F4))
        {
            GlobalPlatformDll.platformShutdown();
        }

        if (state.isDown(KeyboardCode::W))
        {
            GlobalGameState->graphicsDY -= GlobalGameState->scrollSpeed;
        }
        if (state.isDown(KeyboardCode::A))
        {
            GlobalGameState->graphicsDX -= GlobalGameState->scrollSpeed;
        }
        if (state.isDown(KeyboardCode::S))
        {
            GlobalGameState->graphicsDY += GlobalGameState->scrollSpeed;
        }
        if (state.isDown(KeyboardCode::D))
        {
            GlobalGameState->graphicsDX += GlobalGameState->scrollSpeed;
        }
#if DEBUG
        if (state.justPressed(KeyboardCode::P))
        {
            GlobalPlatformDll.DEBUG_platformTogglePause();
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
            GlobalPlatformDll.platformShutdown();
        }
    }

    void handleGamepadInput(GamepadState controllerStates[MAX_NUM_CONTROLLERS])
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
                GlobalPlatformDll.platformShutdown();
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

            float leftTrigger = gamepadState.leftTrigger.end;
            float rightTrigger = gamepadState.rightTrigger.end;
            GlobalPlatformDll.platformVibrateController(controllerIdx, leftTrigger, rightTrigger);

            GlobalGameState->frequencyHz = BaseFrequencyHz + leftStick.x * DeltaFrequencyHz;
        }
    }

    void loadAudio(AudioBuffer buffer)
    {
        loadSineWave(buffer);
        // loadSquareWave(buffer);
    }

    void renderGraphics(const GraphicsBuffer buffer)
    {
        drawColorfulTriangles(buffer);
    }
}
