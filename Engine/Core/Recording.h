#pragma once

#if DEBUG
#include <fstream>
#include <functional>

#include "Input.h"

namespace slurp {
    struct KeyboardState;
    struct MouseState;

    struct RecordingState {
        bool isPaused;
        bool isRecording;
        bool isPlayingBack;
        std::function<void()> onPlaybackEnd;
        std::fstream recordingFileStream;
    };

    void beginRecording(RecordingState& recordingState);

    void recordInput(
        RecordingState& recordingState,
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState gamepadStates[MAX_NUM_GAMEPADS]
    );

    void endRecording(RecordingState& recordingState);

    void beginPlayback(RecordingState& recordingState);

    void readInputRecording(
        RecordingState& recordingState,
        MouseState& outMouseState,
        KeyboardState& outKeyboardState,
        GamepadState outGamepadStates[MAX_NUM_GAMEPADS]
    );
}

#endif
