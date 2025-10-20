#pragma once

#include "CollectionTypes.h"

#include <cstdint>

namespace audio {
    struct PlayingSound;
    struct AudioBuffer;

    class SoundManager {
    public:
        static SoundManager* instance;

        explicit SoundManager();

        /** Game **/
        void setGlobalVolume(float volumeMultiplier);

        void playSound(const asset::Sound* sound);

        void playSound(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop);

        /** Engine **/
        void bufferAudio(const AudioBuffer& buffer);

    private:
        uint32_t _nextSoundId;
        float _globalVolumeMultiplier;
        types::deque_arena<PlayingSound> _loopingQueue;
        types::deque_arena<PlayingSound> _oneShotQueue;
    };

    /** Global Methods **/
    SoundManager* SoundManager::instance = nullptr;

    void setGlobalVolume(float volumeMultiplier);

    void playSound(const asset::Sound* sound);

    void playSound(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop);

    /** Engine **/
    void bufferAudio(const AudioBuffer& buffer);
}
