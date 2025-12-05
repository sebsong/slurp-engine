#pragma once

#include "CollectionTypes.h"
#include "PlayingSound.h"

#include <cstdint>

namespace audio {
    struct AudioBuffer;

    typedef uint32_t sound_id;
    const static sound_id INVALID_SOUND_ID = 0;

    class AudioPlayer {
    public:
        explicit AudioPlayer();

        /** Game **/
        void setGlobalVolume(float volumeMultiplier);

        sound_id play(const asset::Sound* sound);

        sound_id play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop);

        void stop(sound_id id);

        /** Engine **/
        void bufferAudio(const AudioBuffer& buffer);

    private:
        sound_id _nextSoundId;
        float _globalVolumeMultiplier;
        types::deque_arena<PlayingSound> _loopingQueue;
        types::deque_arena<PlayingSound> _oneShotQueue;
    };

    /** Global Methods **/
    inline void setGlobalVolume(float volumeMultiplier) {
        slurp::Globals->AudioPlayer->setGlobalVolume(volumeMultiplier);
    }

    inline sound_id play(const asset::Sound* sound) {
        return slurp::Globals->AudioPlayer->play(sound);
    }

    inline sound_id play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop) {
        return slurp::Globals->AudioPlayer->play(sound, volumeMultiplier, shouldLoop);
    }

    inline void stop(sound_id id) {
        slurp::Globals->AudioPlayer->stop(id);
    }

    inline void bufferAudio(const AudioBuffer& buffer) {
        slurp::Globals->AudioPlayer->bufferAudio(buffer);
    }
}
