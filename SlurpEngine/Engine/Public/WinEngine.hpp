#pragma once

#include <SlurpEngine.hpp>

#include <windows.h>
#include <dsound.h>
#include <unordered_map>
#include <Xinput.h>

typedef uint32_t bool32;

struct WinGraphicsBuffer
{
    BITMAPINFO info;
    void* memory;
    int widthPixels;
    int heightPixels;
    int bytesPerPixel;
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
    int writeAheadSampleCount = samplesPerSec / 100; // NOTE: tuned to the max latency between writeCursor readings.
};

typedef int WinMouseCode;
static const std::unordered_map<WinMouseCode, slurp::MouseCode> MouseWinCodeToSlurpCode =
{
    {VK_LBUTTON, slurp::MouseCode::Left},
    {VK_RBUTTON, slurp::MouseCode::Right},
    {VK_MBUTTON, slurp::MouseCode::Middle},
    {VK_XBUTTON1, slurp::MouseCode::Button1},
    {VK_XBUTTON2, slurp::MouseCode::Button2},
};

typedef WPARAM WinKbCode;
static const std::unordered_map<WinKbCode, slurp::KeyboardCode> KeyboardWinCodeToSlurpCode =
{
    {'W', slurp::KeyboardCode::W},
    {'A', slurp::KeyboardCode::A},
    {'S', slurp::KeyboardCode::S},
    {'D', slurp::KeyboardCode::D},
    {'R', slurp::KeyboardCode::R},
    {'T', slurp::KeyboardCode::T},
    {'P', slurp::KeyboardCode::P},
    {VK_SPACE, slurp::KeyboardCode::SPACE},
    {VK_ESCAPE, slurp::KeyboardCode::ESC},
    {VK_MENU, slurp::KeyboardCode::ALT},
    {VK_F4, slurp::KeyboardCode::F4},
};

typedef uint64_t XInputCode;
static const std::unordered_map<XInputCode, slurp::GamepadCode> GamepadWinCodeToSlurpCode =
{
    {XINPUT_GAMEPAD_DPAD_UP, slurp::GamepadCode::DPAD_UP},
    {XINPUT_GAMEPAD_DPAD_DOWN, slurp::GamepadCode::DPAD_DOWN},
    {XINPUT_GAMEPAD_DPAD_LEFT, slurp::GamepadCode::DPAD_LEFT},
    {XINPUT_GAMEPAD_DPAD_RIGHT, slurp::GamepadCode::DPAD_RIGHT},
    {XINPUT_GAMEPAD_START, slurp::GamepadCode::START},
    {XINPUT_GAMEPAD_BACK, slurp::GamepadCode::BACK},
    {XINPUT_GAMEPAD_LEFT_THUMB, slurp::GamepadCode::LEFT_THUMB},
    {XINPUT_GAMEPAD_RIGHT_THUMB, slurp::GamepadCode::RIGHT_THUMB},
    {XINPUT_GAMEPAD_LEFT_SHOULDER, slurp::GamepadCode::LEFT_SHOULDER},
    {XINPUT_GAMEPAD_RIGHT_SHOULDER, slurp::GamepadCode::RIGHT_SHOULDER},
    {XINPUT_GAMEPAD_A, slurp::GamepadCode::A},
    {XINPUT_GAMEPAD_B, slurp::GamepadCode::B},
    {XINPUT_GAMEPAD_X, slurp::GamepadCode::X},
    {XINPUT_GAMEPAD_Y, slurp::GamepadCode::Y},
};

struct WinTimingInfo
{
    int64_t performanceCounter;
    int64_t performanceCounterFrequency;
};

#if DEBUG
struct WinRecordingState
{
    bool isPaused;
    bool isRecording;
    bool isPlayingBack;
    std::function<void()> onPlaybackEnd;
    HANDLE recordingFileHandle;
};
#endif
