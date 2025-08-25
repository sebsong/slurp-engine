#pragma once
#include "Vector.h"
#include <cstdint>
#include <unordered_map>

#define MAX_NUM_GAMEPADS 4

namespace slurp {
    struct DigitalInputState {
        int transitionCount;
        bool isDown;
    };

    struct AnalogStickInputState {
        Vec2<float> start = {0, 0};
        Vec2<float> end = {0, 0};
    };

    struct AnalogTriggerInputState {
        float start;
        float end;
    };

    enum class MouseCode: uint8_t {
        LeftClick,
        RightClick,
        MiddleClick,
        Button1,
        Button2,
    };

    typedef std::pair<const MouseCode, DigitalInputState> mouse_state_entry;

    struct MouseState {
        Vec2<float> position;
        std::unordered_map<MouseCode, DigitalInputState> state;

        bool getState(MouseCode code, DigitalInputState& outInputState) const {
            if (state.count(code) > 0) {
                outInputState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(MouseCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown;
            }
            return false;
        }

        bool justPressed(MouseCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };

    enum class KeyboardCode: uint8_t {
        W,
        A,
        S,
        D,
        P,
        E,
        R,
        T,
        ESC,
        SPACE,
        ALT,
        F4,
    };

    typedef std::pair<const KeyboardCode, DigitalInputState> keyboard_state_entry;

    struct KeyboardState {
        std::unordered_map<KeyboardCode, DigitalInputState> state;

        bool getState(KeyboardCode code, DigitalInputState& outInputState) const {
            if (state.count(code) > 0) {
                outInputState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(KeyboardCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown;
            }
            return false;
        }

        bool justPressed(KeyboardCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }

        bool justReleased(KeyboardCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return !inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };

    enum class GamepadCode: uint8_t {
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,
        START,
        BACK,
        LEFT_THUMB,
        RIGHT_THUMB,
        LEFT_SHOULDER,
        RIGHT_SHOULDER,
        A,
        B,
        X,
        Y,
    };

    typedef std::pair<const GamepadCode, DigitalInputState> gamepad_state_entry;

    struct GamepadState {
        bool isConnected = false;
        AnalogStickInputState leftStick;
        AnalogStickInputState rightStick;
        AnalogTriggerInputState leftTrigger;
        AnalogTriggerInputState rightTrigger;
        std::unordered_map<GamepadCode, DigitalInputState> state;

        bool getState(GamepadCode code, DigitalInputState& outState) const {
            if (state.count(code) > 0) {
                outState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(GamepadCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown;
            }
            return false;
        }

        bool justPressed(GamepadCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }

        bool justReleased(GamepadCode code) const {
            DigitalInputState inputState;
            if (getState(code, inputState)) {
                return !inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };
}
