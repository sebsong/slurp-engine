#pragma once
#include <cstdint>
#include <map>

#define MAX_NUM_CONTROLLERS 4
#if DEBUG
#define assert(expression) if (!(expression)) { *(int*)(nullptr) = 0; }
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

    enum class KeyboardCode: uint8_t
    {
        W,
        A,
        S,
        D,
        P,
        ESC,
        SPACE,
        ALT,
        F4,
    };

    struct KeyboardState
    {
        std::map<KeyboardCode, DigitalInputState> state;

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

    struct GamepadState
    {
        bool isConnected = false;
        AnalogStickInputState leftStick;
        AnalogStickInputState rightStick;
        AnalogTriggerInputState leftTrigger;
        AnalogTriggerInputState rightTrigger;
        std::map<GamepadCode, DigitalInputState> state;

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

    struct MemoryBlock
    {
        uint64_t sizeBytes;
        void* memory;
    };

    struct GameMemory
    {
        MemoryBlock permanentMemory;
        MemoryBlock transientMemory;
    };

    struct GameState
    {
        float graphicsDX;
        float graphicsDY;
        float scrollSpeed;
        float frequencyHz;
    };

    void init(const GameMemory* gameMemory);
    
    void handleKeyboardInput(KeyboardState state);
    
    void handleGamepadInput(GamepadState controllerStates[MAX_NUM_CONTROLLERS]);

    void loadAudio(int32_t* audioSampleBuffer);

    void renderGraphics(GraphicsBuffer buffer);
}
