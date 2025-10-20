#pragma once

#include "SlurpEngine.h"

#include <SDL2/SDL.h>
#include <unordered_map>

struct MacAudioBuffer {
    SDL_AudioDeviceID device;
    int samplesPerSec;
    int bytesPerSample;
    int bufferSizeBytes;
    int writeAheadSampleCount;
};

typedef int MacMouseCode;
static const std::unordered_map<MacMouseCode, slurp::MouseCode> MouseMacCodeToSlurpCode =
{
    {SDL_BUTTON_LEFT, slurp::MouseCode::LeftClick},
    {SDL_BUTTON_RIGHT, slurp::MouseCode::RightClick},
    {SDL_BUTTON_MIDDLE, slurp::MouseCode::MiddleClick},
    {SDL_BUTTON_X1, slurp::MouseCode::Button1},
    {SDL_BUTTON_X2, slurp::MouseCode::Button2},
};

typedef SDL_Scancode MacKbCode;
static const std::unordered_map<MacKbCode, slurp::KeyboardCode> KeyboardMacCodeToSlurpCode =
{
    {SDL_SCANCODE_W, slurp::KeyboardCode::W},
    {SDL_SCANCODE_A, slurp::KeyboardCode::A},
    {SDL_SCANCODE_S, slurp::KeyboardCode::S},
    {SDL_SCANCODE_D, slurp::KeyboardCode::D},
    {SDL_SCANCODE_E, slurp::KeyboardCode::E},
    {SDL_SCANCODE_R, slurp::KeyboardCode::R},
    {SDL_SCANCODE_T, slurp::KeyboardCode::T},
    {SDL_SCANCODE_P, slurp::KeyboardCode::P},
    {SDL_SCANCODE_SPACE, slurp::KeyboardCode::SPACE},
    {SDL_SCANCODE_ESCAPE, slurp::KeyboardCode::ESC},
    {SDL_SCANCODE_LALT, slurp::KeyboardCode::ALT},
    {SDL_SCANCODE_RALT, slurp::KeyboardCode::ALT},
    {SDL_SCANCODE_F4, slurp::KeyboardCode::F4},
};

typedef uint8_t SDLGamepadCode;
static const std::unordered_map<SDLGamepadCode, slurp::GamepadCode> GamepadMacCodeToSlurpCode =
{
    {SDL_CONTROLLER_BUTTON_DPAD_UP, slurp::GamepadCode::DPAD_UP},
    {SDL_CONTROLLER_BUTTON_DPAD_DOWN, slurp::GamepadCode::DPAD_DOWN},
    {SDL_CONTROLLER_BUTTON_DPAD_LEFT, slurp::GamepadCode::DPAD_LEFT},
    {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, slurp::GamepadCode::DPAD_RIGHT},
    {SDL_CONTROLLER_BUTTON_START, slurp::GamepadCode::START},
    {SDL_CONTROLLER_BUTTON_BACK, slurp::GamepadCode::BACK},
    {SDL_CONTROLLER_BUTTON_LEFTSTICK, slurp::GamepadCode::LEFT_THUMB},
    {SDL_CONTROLLER_BUTTON_RIGHTSTICK, slurp::GamepadCode::RIGHT_THUMB},
    {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, slurp::GamepadCode::LEFT_SHOULDER},
    {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, slurp::GamepadCode::RIGHT_SHOULDER},
    {SDL_CONTROLLER_BUTTON_A, slurp::GamepadCode::A},
    {SDL_CONTROLLER_BUTTON_B, slurp::GamepadCode::B},
    {SDL_CONTROLLER_BUTTON_X, slurp::GamepadCode::X},
    {SDL_CONTROLLER_BUTTON_Y, slurp::GamepadCode::Y},
};

struct MacTimingInfo {
    uint64_t performanceCounter;
    uint64_t performanceCounterFrequency;
};

#if DEBUG
struct MacRecordingState {
    bool isPaused;
    bool isRecording;
    bool isPlayingBack;
    std::function<void()> onPlaybackEnd;
    SDL_RWops* recordingFile;
};
#endif
