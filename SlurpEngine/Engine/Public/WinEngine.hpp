#pragma once

#include <windows.h>
#include <dsound.h>
#include <map>

typedef uint32_t bool32;

struct WinGraphicsBuffer
{
    BITMAPINFO info;
    void* memory;
    int widthPixels;
    int heightPixels;
    int pitchBytes;
};

struct WinScreenDimensions
{
    int x;
    int y;
    int width;
    int height;
};

struct WinAudioBuffer
{
    LPDIRECTSOUNDBUFFER buffer;
    int samplesPerSec = 48000;
    int bytesPerSample = sizeof(int16_t) * 2; // Stereo L + R
    int bufferSizeBytes = samplesPerSec * bytesPerSample;
    int writeAheadSampleCount = samplesPerSec / 20;
    float frequencyHz = 300;
};

static std::map<WPARAM, slurp::KeyboardInputCode> winCodeToSlurpCode =
{
    {'W', slurp::W},
    {'A', slurp::A},
    {'S', slurp::S},
    {'D', slurp::D},
    {VK_SPACE, slurp::SPACE},
    {VK_ESCAPE, slurp::ESC},
    {VK_MENU, slurp::ALT},
    {VK_F4, slurp::F4},
};
