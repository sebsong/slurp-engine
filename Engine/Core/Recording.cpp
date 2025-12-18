#if DEBUG
#include "Recording.h"

#include "MemoryConstructs.h"

namespace slurp {
    void beginRecording(RecordingState& recordingState) {
        recordingState.isRecording = true;
        recordingState.recordingFileStream = std::fstream(
            Globals->PlatformDll->getLocalFilePath(RECORDING_FILE_NAME),
            std::fstream::in | std::fstream::out | std::fstream::trunc
        );
        std::fstream& fileStream = recordingState.recordingFileStream;
        if (!fileStream || !fileStream.is_open()) {
            ASSERT_LOG(false, "Could not open recording file.");
        }
        memory::MemoryBlock recordingMemory = memory::Permanent->getMemoryBlock();
        fileStream.write(
            recordingMemory.memory,
            recordingMemory.size
        );
    }

    template<typename T>
    static void recordStateMap(
        RecordingState& recordingState,
        std::unordered_map<T, DigitalInputState> stateMap
    ) {
        size_t numStates = stateMap.size();
        recordingState.recordingFileStream << numStates;
        for (const std::pair<const T, DigitalInputState>& entry: stateMap) {
            recordingState.recordingFileStream.write(
                reinterpret_cast<const types::byte*>(&entry),
                sizeof(entry)
            );
        }
    }

    static void recordInput(
        RecordingState& recordingState,
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState gamepadStates[MAX_NUM_GAMEPADS]
    ) {
        // record mouse input
        recordingState.recordingFileStream << mouseState.position;
        recordStateMap(recordingState, mouseState.state);

        // record keyboard input
        recordStateMap(recordingState, keyboardState.state);

        // record gamepad input
        for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
            const GamepadState& gamepadState = gamepadStates[gamepadIndex];
            recordingState.recordingFileStream << gamepadState.isConnected;
            recordingState.recordingFileStream.write(
                reinterpret_cast<const types::byte*>(&gamepadState.leftStick),
                sizeof(gamepadState.leftStick)
            );
            recordingState.recordingFileStream.write(
                reinterpret_cast<const types::byte*>(&gamepadState.rightStick),
                sizeof(gamepadState.rightStick)
            );
            recordingState.recordingFileStream.write(
                reinterpret_cast<const types::byte*>(&gamepadState.leftTrigger),
                sizeof(gamepadState.leftTrigger)
            );
            recordingState.recordingFileStream.write(
                reinterpret_cast<const types::byte*>(&gamepadState.rightTrigger),
                sizeof(gamepadState.rightTrigger)
            );
            recordStateMap(recordingState, gamepadState.state);
        }
    }

    void endRecording(RecordingState& recordingState) {
        recordingState.recordingFileStream.close();
        recordingState.isRecording = false;
    }

    void beginPlayback(RecordingState& recordingState) {
        recordingState.recordingFileStream = std::fstream(
            Globals->PlatformDll->getLocalFilePath(RECORDING_FILE_NAME)
        );
        std::fstream& fileStream = recordingState.recordingFileStream;
        if (!fileStream || !fileStream.is_open()) {
            ASSERT_LOG(false, "Could not open recording file.");
        }
        memory::MemoryBlock recordingMemory = memory::Permanent->getMemoryBlock();

        fileStream.read(recordingMemory.memory, recordingMemory.size);
        recordingState.isPlayingBack = true;
        auto onPlaybackEnd = [&recordingState]() -> void { recordingState.isPlayingBack = false; };
        recordingState.onPlaybackEnd = onPlaybackEnd;
    }

    template<typename T>
    void readInputStateMap(
        RecordingState& recordingState,
        std::unordered_map<T, DigitalInputState>& outStateMap
    ) {
        size_t numStates = 0;
        recordingState.recordingFileStream >> numStates;
        outStateMap.clear();
        for (size_t i = 0; i < numStates; i++) {
            std::pair<const T, DigitalInputState> entry;
            recordingState.recordingFileStream.read(
                reinterpret_cast<types::byte*>(&entry),
                sizeof(entry)
            );
            outStateMap.insert(entry);
        }
    }

    static void readInputRecording(
        RecordingState& recordingState,
        MouseState& outMouseState,
        KeyboardState& outKeyboardState,
        GamepadState outGamepadStates[MAX_NUM_GAMEPADS]
    ) {
        if (recordingState.recordingFileStream.eof()) {
            recordingState.isPlayingBack = false;
            recordingState.recordingFileStream.close();
            recordingState.onPlaybackEnd();
        }

        // read mouse input
        recordingState.recordingFileStream.read(
            reinterpret_cast<types::byte*>(&outMouseState.position),
            sizeof(outMouseState.position)
        );
        readInputStateMap(recordingState, outMouseState.state);

        // read keyboard input
        readInputStateMap(recordingState, outKeyboardState.state);

        // read gamepad input
        for (int gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
            GamepadState& outGamepadState = outGamepadStates[gamepadIndex];
            recordingState.recordingFileStream >> outGamepadState.isConnected;

            recordingState.recordingFileStream.read(
                reinterpret_cast<types::byte*>(&outGamepadState.leftStick),
                sizeof(outGamepadState.leftStick)
            );
            recordingState.recordingFileStream.read(
                reinterpret_cast<types::byte*>(&outGamepadState.rightStick),
                sizeof(outGamepadState.rightStick)
            );
            recordingState.recordingFileStream.read(
                reinterpret_cast<types::byte*>(&outGamepadState.leftTrigger),
                sizeof(outGamepadState.leftTrigger)
            );
            recordingState.recordingFileStream.read(
                reinterpret_cast<types::byte*>(&outGamepadState.rightTrigger),
                sizeof(outGamepadState.rightTrigger)
            );
            readInputStateMap(recordingState, outGamepadState.state);
        }
    }
}

#endif
