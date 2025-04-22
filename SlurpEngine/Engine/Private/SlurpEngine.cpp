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
    static float lowScrollSpeed = 1;
    static float highScrollSpeed = 5;
    static float scrollSpeed = lowScrollSpeed;

    static float frequencyHz;

    static void loadSineWave(AudioBuffer buffer)
    {
        static float tSine = 0;
        float sineWavePeriod = buffer.samplesPerSec / buffer.frequencyHz;

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
        float squareWavePeriod = buffer.samplesPerSec / buffer.frequencyHz;

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

    void handleKeyboardInput(KeyboardInputState state)
    {
        if (state.isDown(KeyboardInputCode::ALT) && state.isDown(KeyboardInputCode::F4))
        {
            GlobalIsRunning = false;
        }

        if (state.isDown(KeyboardInputCode::W))
        {
            graphicsDY -= scrollSpeed;
        }
        if (state.isDown(KeyboardInputCode::A))
        {
           graphicsDX -= scrollSpeed;
        }
        if (state.isDown(KeyboardInputCode::S))
        {
            graphicsDY += scrollSpeed;
        }
        if (state.isDown(KeyboardInputCode::D))
        {
            graphicsDX += scrollSpeed;
        }

        DigitalInputState inputState;
        if (state.getState(KeyboardInputCode::SPACE, inputState))
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

        std::cout << "DX: " << graphicsDX << std::endl;

        if (state.isDown(KeyboardInputCode::ESC))
        {
            GlobalIsRunning = false;
        }
    }

    void handleGamepadInput(KeyboardInputCode code)
    {
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
