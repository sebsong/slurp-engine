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

        void setGlobalVolume(float volumeMultiplier);

        void playSound(const Sound& sound);

        void playSound(const Sound& sound, float volumeMultiplier, bool shouldLoop);

        void loadAudio(const AudioBuffer& buffer);

    private:
        uint32_t _nextSoundId;
        float _globalVolumeMultiplier;
        std::deque<PlayingSound> _queue;
    };
}
