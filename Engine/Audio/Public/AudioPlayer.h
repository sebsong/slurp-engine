#pragma once

#include "CollectionTypes.h"

#include <cstdint>

namespace audio {
    struct PlayingSound;
    struct AudioBuffer;

    class AudioPlayer {
    public:
        explicit AudioPlayer();

        /** Game **/
        void setGlobalVolume(float volumeMultiplier);

        void play(const asset::Sound* sound);

        void play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop);

        /** Engine **/
        void bufferAudio(const AudioBuffer& buffer);

    private:
        uint32_t _nextSoundId;
        float _globalVolumeMultiplier;
        types::deque_arena<PlayingSound> _loopingQueue;
        types::deque_arena<PlayingSound> _oneShotQueue;
    };

    /** Global Methods **/
    void setGlobalVolume(float volumeMultiplier);

    void play(const asset::Sound* sound);

    void play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop);

    /** Engine **/
    void bufferAudio(const AudioBuffer& buffer);
}
