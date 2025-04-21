#pragma once

#include <windows.h>
#include <dsound.h>
#include <map>

typedef int32_t bool32;

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

static std::map<WPARAM, slurp::InputCode> winCodeToSlurpCode =
{
    {'W', slurp::KB_W},
    {'A', slurp::KB_A},
    {'S', slurp::KB_S},
    {'D', slurp::KB_D},
    {VK_SPACE, slurp::KB_SPACE},
    {VK_ESCAPE, slurp::KB_ESC},
};
