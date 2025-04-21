#pragma once
#include <cstdint>
#include <map>
#include <vector>

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
    
    enum KeyboardInputCode: uint8_t
    {
        W,
        A,
        S,
        D,
        ESC,
        SPACE,
    };

    struct DigitalInputState
    {
        int transitionCount;
        bool isDown;
    };
    
    struct AnalogInputState
    {
        std::vector<float> startXY[2];
        std::vector<float> endXY[2];
        std::vector<float> minXY[2];
        std::vector<float> maxXY[2];
    };

    struct KeyboardInputState
    {
        std::map<KeyboardInputCode, DigitalInputState> state;
        bool hasInput(KeyboardInputCode code) const
        {
            return state.count(code) > 0;
        }
        DigitalInputState getInputState(KeyboardInputCode code) const
        {
            return state.at(code);
        }
    };

    void loadAudio(int32_t* audioSampleBuffer);

    void renderGraphics(GraphicsBuffer buffer);
    
    void main(bool& isRunning);
}
