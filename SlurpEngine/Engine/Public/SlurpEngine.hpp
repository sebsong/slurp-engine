#pragma once

#include <DynamicDeclaration.hpp>
#include <Platform.hpp>

#include <cstdint>
#include <unordered_map>

#define MAX_NUM_CONTROLLERS 4
#if DEBUG
#define assert(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define assert(expression) 
#endif

namespace slurp
{
    struct AudioBuffer
    {
        int32_t* samples; // 16-bit Stereo L + R samples
        int samplesPerSec;
        int samplesToWrite;
    };

    struct GraphicsBuffer
    {
        void* memory; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
        int pitchBytes;
    };

    struct DigitalInputState
    {
        int transitionCount;
        bool isDown;
    };

    struct XYCoord
    {
        float x;
        float y;
    };

    struct AnalogStickInputState
    {
        XYCoord startXY = {0, 0};
        XYCoord endXY = {0, 0};
    };

    struct AnalogTriggerInputState
    {
        float start;
        float end;
    };
    
    enum class MouseCode: uint8_t
    {
        Left,
        Right,
        Middle,
        Button1,
        Button2,
    };

    struct MouseState
    {
        float x;
        float y;
        std::unordered_map<MouseCode, DigitalInputState> state;
        bool getState(MouseCode code, DigitalInputState& outInputState) const
        {
            if (state.count(code) > 0)
            {
                outInputState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(MouseCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown;
            }
            return false;
        }

        bool justPressed(MouseCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };

    enum class KeyboardCode: uint8_t
    {
        W,
        A,
        S,
        D,
        P,
        R,
        T,
        ESC,
        SPACE,
        ALT,
        F4,
    };

    typedef std::pair<const slurp::KeyboardCode, slurp::DigitalInputState> keyboard_state_entry;
    struct KeyboardState
    {
        std::unordered_map<KeyboardCode, DigitalInputState> state;

        bool getState(KeyboardCode code, DigitalInputState& outInputState) const
        {
            if (state.count(code) > 0)
            {
                outInputState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(KeyboardCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown;
            }
            return false;
        }

        bool justPressed(KeyboardCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };

    enum class GamepadCode: uint8_t
    {
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

    typedef std::pair<const slurp::GamepadCode, slurp::DigitalInputState> gamepad_state_entry;

    struct GamepadState
    {
        bool isConnected = false;
        AnalogStickInputState leftStick;
        AnalogStickInputState rightStick;
        AnalogTriggerInputState leftTrigger;
        AnalogTriggerInputState rightTrigger;
        std::unordered_map<GamepadCode, DigitalInputState> state;

        bool getState(GamepadCode code, DigitalInputState& outState) const
        {
            if (state.count(code) > 0)
            {
                outState = state.at(code);
                return true;
            }
            return false;
        }

        bool isDown(GamepadCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown;
            }
            return false;
        }
    };

    struct GameState
    {
        float graphicsDX;
        float graphicsDY;
        float scrollSpeed;
        float frequencyHz;
        float tWave;
        float playerX;
        float playerY;
        float mouseX;
        float mouseY;
        
        bool mouseL;
        bool mouseR;
        bool mouseM;
        bool mouseB1;
        bool mouseB2;
    };

#if DEBUG
    struct RecordingState
    {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(const platform::PlatformDll platformDll, platform::GameMemory* gameMemory)
#define SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(fnName) void fnName(slurp::MouseState mouseState, slurp::KeyboardState keyboardState)
#define SLURP_HANDLE_GAMEPAD_INPUT(fnName) void fnName(slurp::GamepadState controllerStates[MAX_NUM_CONTROLLERS])
#define SLURP_LOAD_AUDIO(fnName) void fnName(slurp::AudioBuffer buffer)
#define SLURP_RENDER_GRAPHICS(fnName) void fnName(slurp::GraphicsBuffer buffer)
#define SLURP_UPDATE(fnName) void fnName()

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT, handleMouseAndKeyboardInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_GAMEPAD_INPUT, handleGamepadInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_LOAD_AUDIO, loadAudio)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_RENDER_GRAPHICS, renderGraphics)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE, update)

    struct SlurpDll
    {
        dyn_init* init = stub_init;
        dyn_handleMouseAndKeyboardInput* handleMouseAndKeyboardInput = stub_handleMouseAndKeyboardInput;
        dyn_handleGamepadInput* handleGamepadInput = stub_handleGamepadInput;
        dyn_loadAudio* loadAudio = stub_loadAudio;
        dyn_renderGraphics* renderGraphics = stub_renderGraphics;
        dyn_update* update = stub_update;
    };
}
