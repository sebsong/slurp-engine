#pragma once

#if DEBUG
#include <fstream>
#include <functional>

namespace slurp {
    struct RecordingState {
        bool isPaused;
        bool isRecording;
        bool isPlayingBack;
        std::function<void()> onPlaybackEnd;
        std::fstream recordingFileStream;
    };

    void beginRecording(RecordingState* recordingState);

    void endRecording(RecordingState* recordingState);

    void beginPlayback(RecordingState* recordingState);
}
#endif
