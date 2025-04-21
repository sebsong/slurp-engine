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
    };
    
    enum InputCode
    {
        KB_W,
        KB_A,
        KB_S,
        KB_D,
        KB_ESC,
        KB_SPACE,
    };

    void loadAudio(int32_t* audioSampleBuffer);

    void renderGraphics(GraphicsBuffer buffer);
}
