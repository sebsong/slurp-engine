#pragma once
#include <cstdint>
#include <deque>

namespace audio {
    struct PlayingSound;
    struct AudioBuffer;
    struct Sound;

    class SoundManager {
    public:
        explicit SoundManager();

        void play(const Sound& sound, float volumeMultiplier, bool shouldLoop);

        void loadAudio(const AudioBuffer& buffer);

    private:
        uint32_t _nextSoundId;
        std::deque<PlayingSound> _queue;
    };
}
