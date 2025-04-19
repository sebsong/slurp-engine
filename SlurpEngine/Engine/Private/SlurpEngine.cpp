#include <SlurpEngine.hpp>

#include <iostream>

typedef unsigned char byte;

static constexpr float Pi = 3.14159265359f;

static constexpr int GlobalVolume = 0.1 * 32000;
static constexpr int GlobalFrequencyHz = 300;

void slurpMain()
{
}

static void loadCoolAudio(AudioBuffer buffer)
{
    static float tSine = 0;
    float sineWavePeriod = buffer.samplesPerSec / GlobalFrequencyHz;

    long regionNumSamples = buffer.numBytesToWrite / buffer.bytesPerSample;
    int16_t* buf = reinterpret_cast<int16_t*>(buffer.samples);
    for (int regionSampleIndex = 0; regionSampleIndex < regionNumSamples; regionSampleIndex++)
    {
        int16_t subSampleData = sinf(tSine) * GlobalVolume;
        // *buffer.samples++ = (subSampleData << 16) | subSampleData;
        *buf++ = subSampleData;
        *buf++ = subSampleData;

        tSine += 2 * Pi / sineWavePeriod;
    }
}

void loadAudio(AudioBuffer buffer)
{
    loadCoolAudio(buffer);
}

static void renderCoolGraphics(const GraphicsBuffer buffer, float xOffset, float yOffset)
{
    byte* bitmapBytes = static_cast<byte*>(buffer.memory);
    for (int y = 0; y < buffer.heightPixels; y++)
    {
        uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
        for (int x = 0; x < buffer.widthPixels; x++)
        {
            uint8_t r = y + yOffset;
            uint8_t g = (x + xOffset) - (y + yOffset);
            uint8_t b = x + xOffset;

            uint32_t pixel = (r << 16) | (g << 8) | b;
            *rowPixels++ = pixel;
        }

        bitmapBytes += buffer.pitchBytes;
    }
}

void renderGraphics(const GraphicsBuffer buffer, float xOffset, float yOffset)
{
    renderCoolGraphics(buffer, xOffset, yOffset);
}
