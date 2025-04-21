#include <SlurpEngine.hpp>
#include <iostream>

#include "WinEngine.hpp"

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;
static constexpr int GlobalVolume = 0.1 * 32000;

namespace slurp
{
    static bool GlobalIsRunning = true;
    
    static float dX = 0;
    static float dY = 0;
    static float ddX = 0;
    static float ddY = 0;
    static float scrollSpeed = 255;

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
        dX += scrollSpeed * ddX / 5000;
        dY += scrollSpeed * ddY / 5000;
        
        byte* bitmapBytes = static_cast<byte*>(buffer.memory);
        for (int y = 0; y < buffer.heightPixels; y++)
        {
            uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
            for (int x = 0; x < buffer.widthPixels; x++)
            {
                uint8_t r = y + dY;
                uint8_t g = (x + dX) - (y + dY);
                uint8_t b = x + dX;

                uint32_t pixel = (r << 16) | (g << 8) | b;
                *rowPixels++ = pixel;
            }

            bitmapBytes += buffer.pitchBytes;
        }
    }

    void handleKeyboardInput(KeyboardInputState state)
    {
        // if (alt&& isDown && virtualKeyCode
        // ==
        // VK_F4
        // )
        // {
        //     GlobalRunning = false;
        // }

        if (state.hasInput(KeyboardInputCode::W))
        {
            DigitalInputState inputState = state.getInputState(KeyboardInputCode::W);
            if (inputState.isDown)
            {
                ddY -= 1;
            } else
            {
                ddY += 1;
            }
        }
        
        if (state.hasInput(KeyboardInputCode::S))
        {
            DigitalInputState inputState = state.getInputState(KeyboardInputCode::S);
            if (inputState.isDown)
            {
                ddY += 1;
            } else
            {
                ddY -= 1;
            }
        }
        
        if (state.hasInput(KeyboardInputCode::ESC))
        {
            DigitalInputState inputState = state.getInputState(KeyboardInputCode::ESC);
            if (inputState.isDown)
            {
                GlobalIsRunning = false;
            }
        }

        // switch (code)
        // {
        // case W:
        //     {
        //         if (isDown)
        //         {
        //             ddY -= 1;
        //         }
        //         else
        //         {
        //             ddY += 1;
        //         }
        //     }
        //     break;
        // case A:
        //     {
        //         if (isDown)
        //         {
        //             ddX -= 1;
        //         }
        //         else
        //         {
        //             ddX += 1;
        //         }
        //     }
        //     break;
        // case S:
        //     {
        //         if (isDown)
        //         {
        //             ddY += 1;
        //         }
        //         else
        //         {
        //             ddY -= 1;
        //         }
        //     }
        //     break;
        // case D:
        //     {
        //         if (isDown)
        //         {
        //             ddX += 1;
        //         }
        //         else
        //         {
        //             ddX -= 1;
        //         }
        //     }
        //     break;
        // case ESC:
        //     {
        //         GlobalIsRunning = false;
        //     }
        //     break;
        // case SPACE:
        //     {
        //         if (isDown)
        //         {
        //             scrollSpeed *= 5;
        //         }
        //         else
        //         {
        //             scrollSpeed /= 5;
        //         }
        //     }
        //     break;
        // default:
        //     {
        //     }
        //     break;
        // }
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
