#pragma once
#include <cstdint>

struct AudioBuffer
{
    int32_t* samples;
    int samplesPerSec;
    int numBytesToWrite;
    int bytesPerSample;
};

struct GraphicsBuffer
{
    void* memory; // memory byte order: XRGB
    int widthPixels;
    int heightPixels;
    int pitchBytes;
};

void main();

void loadAudio(int32_t* audioSampleBuffer);

void renderGraphics(GraphicsBuffer buffer, float xOffset, float yOffset);
