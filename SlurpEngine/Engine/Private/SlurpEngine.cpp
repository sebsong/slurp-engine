#include <SlurpEngine.hpp>
#include <iostream>

#include "WinEngine.hpp"

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr int GlobalVolume = 0.1 * 32000;

namespace slurp
{
    static bool GlobalIsRunning = true;

    static float graphicsDX = 0;
    static float graphicsDY = 0;
    
    static const float lowScrollSpeed = 1;
    static const float highScrollSpeed = 5;
    static float scrollSpeed = lowScrollSpeed;

    static const float baseFrequencyHz = 360;
    static const float deltaFrequencyHz = 220;
    static float GlobalFrequencyHz = baseFrequencyHz;

    static void loadSineWave(AudioBuffer buffer)
    {
        static float tSine = 0;
        float sineWavePeriod = buffer.samplesPerSec / GlobalFrequencyHz;

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
        float squareWavePeriod = buffer.samplesPerSec / GlobalFrequencyHz;

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
                uint8_t r = y + graphicsDY;
                uint8_t g = (x + graphicsDX) - (y + graphicsDY);
                uint8_t b = x + graphicsDX;

                uint32_t pixel = (r << 16) | (g << 8) | b;
                *rowPixels++ = pixel;
            }

            bitmapBytes += buffer.pitchBytes;
        }
    }

    void handleKeyboardInput(KeyboardState state)
    {
        if (state.isDown(KeyboardCode::ALT) && state.isDown(KeyboardCode::F4))
        {
            GlobalIsRunning = false;
        }

        if (state.isDown(KeyboardCode::W))
        {
            graphicsDY -= scrollSpeed;
        }
        if (state.isDown(KeyboardCode::A))
        {
            graphicsDX -= scrollSpeed;
        }
        if (state.isDown(KeyboardCode::S))
        {
            graphicsDY += scrollSpeed;
        }
        if (state.isDown(KeyboardCode::D))
        {
            graphicsDX += scrollSpeed;
        }

        DigitalInputState inputState;
        if (state.getState(KeyboardCode::SPACE, inputState))
        {
            if (inputState.isDown)
            {
                scrollSpeed = 5;
            }
            else if (!inputState.isDown)
            {
                scrollSpeed = 1;
            }
        }

        if (state.isDown(KeyboardCode::ESC))
        {
            GlobalIsRunning = false;
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
                GlobalIsRunning = false;
            }

            if (gamepadState.isDown(GamepadCode::LEFT_SHOULDER) || gamepadState.isDown(GamepadCode::RIGHT_SHOULDER))
            {
                scrollSpeed = highScrollSpeed;
            }
            else
            {
                scrollSpeed = lowScrollSpeed;
            }

            XYCoord leftStickMax = gamepadState.leftStick.maxXY;
            graphicsDX += leftStickMax.x * scrollSpeed;
            graphicsDY -= leftStickMax.y * scrollSpeed;

            float leftTriggerMax = gamepadState.leftTrigger.max;
            float rightTriggerMax = gamepadState.rightTrigger.max;
            platformVibrateController(controllerIdx, leftTriggerMax, rightTriggerMax);

            XYCoord leftStickEnd = gamepadState.leftStick.endXY;
            GlobalFrequencyHz = baseFrequencyHz + leftStickEnd.x * deltaFrequencyHz;
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

    void main(bool& isRunning)
    {
        if (!GlobalIsRunning)
        {
            isRunning = false;
        }
    }
}
