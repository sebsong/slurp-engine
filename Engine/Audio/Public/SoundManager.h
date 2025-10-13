#pragma once
#include <cstdint>
#include <deque>

namespace asset {
    struct PlayingSound;
}

namespace audio {
    struct PlayingSound;
    struct AudioBuffer;

    class SoundManager {
    public:
        explicit SoundManager();

        /** Game **/
        void setGlobalVolume(float volumeMultiplier);

        void playSound(const asset::PlayingSound* sound);

        void playSound(const asset::PlayingSound* sound, float volumeMultiplier, bool shouldLoop);

        /** Engine **/
        void bufferAudio(const AudioBuffer& buffer);

    private:
        uint32_t _nextSoundId;
        float _globalVolumeMultiplier;
        std::deque<PlayingSound> _loopingQueue;
        std::deque<PlayingSound> _oneShotQueue;
    };
}
