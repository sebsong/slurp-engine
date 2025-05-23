#pragma once

#include <Platform.hpp>
#include <DynamicDeclaration.hpp>
#include <Vector.hpp>

#include <cstdint>
#include <array>
#include <unordered_map>

#define MAX_NUM_CONTROLLERS 4
#if DEBUG
#define assert(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define assert(expression) 
#endif

#define COLOR_PALETTE_SIZE 8
#define TILEMAP_WIDTH 32
#define TILEMAP_HEIGHT 18

namespace slurp
{
    typedef uint8_t ColorPaletteIdx;
    typedef uint32_t Pixel;

    struct AudioBuffer
    {
        int32_t* samples; // 16-bit Stereo L + R samples
        int samplesPerSec;
        int samplesToWrite;
    };


    struct GraphicsBuffer
    {
        Pixel* const pixelMap; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
    };


    struct ColorPalette
    {
        Pixel colors[COLOR_PALETTE_SIZE];
    };


    struct DigitalInputState
    {
        int transitionCount;
        bool isDown;
    };

    struct AnalogStickInputState
    {
        Vector2<float> start = {0, 0};
        Vector2<float> end = {0, 0};
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
        Vector2<int> position;
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

        bool justReleased(KeyboardCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return !inputState.isDown && inputState.transitionCount != 0;
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

        bool justPressed(GamepadCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }

        bool justReleased(GamepadCode code) const
        {
            DigitalInputState inputState;
            if (getState(code, inputState))
            {
                return !inputState.isDown && inputState.transitionCount != 0;
            }
            return false;
        }
    };

    struct Entity
    {
        int size;
        float speed;
        Vector2<int> position;
        Vector2<float> direction;
        Vector2<int> relativeCollisionPoints[4];
    };

    struct Tilemap
    {
        std::array<std::array<ColorPaletteIdx, TILEMAP_WIDTH>, TILEMAP_HEIGHT> map;
        uint8_t tileSize;
    };

    struct GameState
    {
        Tilemap tilemap;
        bool isInitialized;
        ColorPalette colorPalette;
        Entity player;
        Vector2<int> mousePosition;
    };

#if DEBUG
    struct RecordingState
    {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(const platform::PlatformDll platformDll, platform::GameMemory* gameMemory)
#define SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(fnName) void fnName(slurp::MouseState mouseState, slurp::KeyboardState keyboardState, float dt)
#define SLURP_HANDLE_GAMEPAD_INPUT(fnName) void fnName(slurp::GamepadState controllerStates[MAX_NUM_CONTROLLERS], float dt)
#define SLURP_LOAD_AUDIO(fnName) void fnName(slurp::AudioBuffer buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(slurp::GraphicsBuffer buffer, float dt)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT, handleMouseAndKeyboardInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_GAMEPAD_INPUT, handleGamepadInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_LOAD_AUDIO, loadAudio)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, updateAndRender)

    struct SlurpDll
    {
        dyn_init* init = stub_init;
        dyn_handleMouseAndKeyboardInput* handleMouseAndKeyboardInput = stub_handleMouseAndKeyboardInput;
        dyn_handleGamepadInput* handleGamepadInput = stub_handleGamepadInput;
        dyn_loadAudio* loadAudio = stub_loadAudio;
        dyn_updateAndRender* updateAndRender = stub_updateAndRender;
    };
}
