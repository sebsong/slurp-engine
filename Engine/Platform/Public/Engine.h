#pragma once
#include <functional>

#include "Input.h"

#include <unordered_map>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_gamepad.h>

typedef uint8_t SDL_MouseButtonCode;
static const std::unordered_map<SDL_MouseButtonCode, slurp::MouseCode> MouseButtonSDLCodeToSlurpCode =
{
    {SDL_BUTTON_LEFT, slurp::MouseCode::LeftClick},
    {SDL_BUTTON_RIGHT, slurp::MouseCode::RightClick},
    {SDL_BUTTON_MIDDLE, slurp::MouseCode::MiddleClick},
    {SDL_BUTTON_X1, slurp::MouseCode::Button1},
    {SDL_BUTTON_X2, slurp::MouseCode::Button2},
};

static const std::unordered_map<SDL_Keycode, slurp::KeyboardCode> KeyboardSDLCodeToSlurpCode =
{
    {SDLK_W, slurp::KeyboardCode::W},
    {SDLK_A, slurp::KeyboardCode::A},
    {SDLK_S, slurp::KeyboardCode::S},
    {SDLK_D, slurp::KeyboardCode::D},
    {SDLK_E, slurp::KeyboardCode::E},
    {SDLK_R, slurp::KeyboardCode::R},
    {SDLK_T, slurp::KeyboardCode::T},
    {SDLK_P, slurp::KeyboardCode::P},
    {SDLK_0, slurp::KeyboardCode::NUM_0},
    {SDLK_1, slurp::KeyboardCode::NUM_1},
    {SDLK_2, slurp::KeyboardCode::NUM_2},
    {SDLK_3, slurp::KeyboardCode::NUM_3},
    {SDLK_4, slurp::KeyboardCode::NUM_4},
    {SDLK_5, slurp::KeyboardCode::NUM_5},
    {SDLK_6, slurp::KeyboardCode::NUM_6},
    {SDLK_7, slurp::KeyboardCode::NUM_7},
    {SDLK_8, slurp::KeyboardCode::NUM_8},
    {SDLK_9, slurp::KeyboardCode::NUM_9},
    {SDLK_SPACE, slurp::KeyboardCode::SPACE},
    {SDLK_ESCAPE, slurp::KeyboardCode::ESC},
    {SDL_KMOD_ALT, slurp::KeyboardCode::ALT},
    {SDLK_TAB, slurp::KeyboardCode::TAB},
    {SDLK_F4, slurp::KeyboardCode::F4},
};

typedef uint8_t SDL_GamepadButtonCode;
static const std::unordered_map<SDL_GamepadButtonCode, slurp::GamepadCode> GamepadButtonSDLCodeToSlurpCode =
{
    {SDL_GAMEPAD_BUTTON_DPAD_UP, slurp::GamepadCode::DPAD_UP},
    {SDL_GAMEPAD_BUTTON_DPAD_DOWN, slurp::GamepadCode::DPAD_DOWN},
    {SDL_GAMEPAD_BUTTON_DPAD_LEFT, slurp::GamepadCode::DPAD_LEFT},
    {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, slurp::GamepadCode::DPAD_RIGHT},
    {SDL_GAMEPAD_BUTTON_START, slurp::GamepadCode::START},
    {SDL_GAMEPAD_BUTTON_BACK, slurp::GamepadCode::BACK},
    {SDL_GAMEPAD_BUTTON_LEFT_STICK, slurp::GamepadCode::LEFT_STICK},
    {SDL_GAMEPAD_BUTTON_RIGHT_STICK, slurp::GamepadCode::RIGHT_STICK},
    {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, slurp::GamepadCode::LEFT_SHOULDER},
    {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, slurp::GamepadCode::RIGHT_SHOULDER},
    {SDL_GAMEPAD_BUTTON_NORTH, slurp::GamepadCode::X},
    {SDL_GAMEPAD_BUTTON_EAST, slurp::GamepadCode::A},
    {SDL_GAMEPAD_BUTTON_SOUTH, slurp::GamepadCode::B},
    {SDL_GAMEPAD_BUTTON_WEST, slurp::GamepadCode::Y},
};
