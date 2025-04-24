#include <SlurpEngine.hpp>
#include <iostream>

#include "WinEngine.hpp"

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr int GlobalVolume = 0.1 * 32000;

namespace slurp
{
    static GameState* GlobalGameState;
    
    static constexpr float LowScrollSpeed = 1;
    static constexpr float HighScrollSpeed = 5;
    static constexpr float BaseFrequencyHz = 360;
    static constexpr float DeltaFrequencyHz = 220;

    static void loadSineWave(AudioBuffer buffer)
    {
        static float tSine = 0;
        float sineWavePeriod = buffer.samplesPerSec / GlobalGameState->frequencyHz;

        int16_t* subSamples = reinterpret_cast<int16_t*>(buffer.samples);
        for (int regionSampleIndex = 0; regionSampleIndex < buffer.samplesToWrite; regionSampleIndex++)
        {
            int16_t subSampleData = sinf(tSine) * GlobalVolume;
            // *buffer.samples++ = (subSampleData << 16) | subSampleData;
            *subSamples++ = subSampleData;
            *subSamples++ = subSampleData;

            tSine += 2 * Pi / sineWavePeriod;
        }
    }

    static void loadSquareWave(AudioBuffer buffer)
    {
        static float tSquare = 0;
        float squareWavePeriod = buffer.samplesPerSec / GlobalGameState->frequencyHz;

        int16_t* subSamples = reinterpret_cast<int16_t*>(buffer.samples);
        for (int regionSampleIndex = 0; regionSampleIndex < buffer.samplesToWrite; regionSampleIndex++)
        {
            int16_t square = ((int)tSquare % 2 == 0) ? 1 : -1;
            int16_t sampleData = square * GlobalVolume / 4; // artificially lower volume
            *subSamples++ = sampleData;
            *subSamples++ = sampleData;

            tSquare += 1 / (squareWavePeriod / 2.f);
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
                uint8_t r = y + GlobalGameState->graphicsDY;
                uint8_t g = (x + GlobalGameState->graphicsDX) - (y + GlobalGameState->graphicsDY);
                uint8_t b = x + GlobalGameState->graphicsDX;

                uint32_t pixel = (r << 16) | (g << 8) | b;
                *rowPixels++ = pixel;
            }

            bitmapBytes += buffer.pitchBytes;
        }
    }

    void init(GameMemory* gameMemory)
    {
        assert(sizeof(GameState) <= gameMemory->permanentMemory.sizeBytes)
        GlobalGameState = static_cast<GameState*>(gameMemory->permanentMemory.memory);
        GlobalGameState->scrollSpeed = LowScrollSpeed;
        GlobalGameState->frequencyHz = BaseFrequencyHz;
    }

    void handleKeyboardInput(KeyboardState state)
    {
        if (state.isDown(KeyboardCode::ALT) && state.isDown(KeyboardCode::F4))
        {
            platformShutdown();
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
            platformShutdown();
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
                platformShutdown();
            }

            if (gamepadState.isDown(GamepadCode::LEFT_SHOULDER) || gamepadState.isDown(GamepadCode::RIGHT_SHOULDER))
            {
                GlobalGameState->scrollSpeed = HighScrollSpeed;
            }
            else
            {
                GlobalGameState->scrollSpeed = LowScrollSpeed;
            }

            XYCoord leftStickMax = gamepadState.leftStick.maxXY;
            GlobalGameState->graphicsDX += leftStickMax.x * GlobalGameState->scrollSpeed;
            GlobalGameState->graphicsDY -= leftStickMax.y * GlobalGameState->scrollSpeed;

            float leftTriggerMax = gamepadState.leftTrigger.max;
            float rightTriggerMax = gamepadState.rightTrigger.max;
            platformVibrateController(controllerIdx, leftTriggerMax, rightTriggerMax);

            XYCoord leftStickEnd = gamepadState.leftStick.endXY;
            GlobalGameState->frequencyHz = BaseFrequencyHz + leftStickEnd.x * DeltaFrequencyHz;
        }
    }

    void loadAudio(AudioBuffer buffer)
    {
        loadSineWave(buffer);
    }

    void renderGraphics(const GraphicsBuffer buffer)
    {
        drawColorfulTriangles(buffer);
    }
}
