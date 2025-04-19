#pragma once
#include <cstdint>

namespace slurp
{
    struct AudioBuffer
    {
        int32_t* samples; // 16-bit Stereo L + R samples
        int samplesPerSec;
        int samplesToWrite;
        float frequencyHz; // TODO: don't pass this in once input control is extracted
    };

    struct GraphicsBuffer
    {
        void* memory; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
        int pitchBytes;
        float dX; // TODO: don't pass this in once input control is extracted
        float dY; // TODO: don't pass this in once input control is extracted
    };

    void loadAudio(int32_t* audioSampleBuffer);

    void renderGraphics(GraphicsBuffer buffer);
}
