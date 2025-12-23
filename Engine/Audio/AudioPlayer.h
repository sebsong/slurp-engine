#pragma once

#include "CollectionTypes.h"
#include "PlayingSound.h"

struct MIX_Mixer;
struct MIX_Track;

namespace audio {
    struct AudioBuffer;

    typedef uint32_t sound_id;
    static constexpr sound_id INVALID_SOUND_ID = 0;

    class AudioPlayer {
    public:
        explicit AudioPlayer(MIX_Mixer* audioMixer);

        void setGlobalVolume(float volumeMultiplier);

        sound_id play(
            const asset::Sound* sound,
            float volumeMultiplier,
            bool shouldLoop,
            const std::function<void()>& onFinish
        );

        void stop(sound_id id);

        void stop(MIX_Track* audioTrack);

        void clearAll();

    private:
        sound_id _nextSoundId;
        float _globalVolumeMultiplier;
        MIX_Mixer* _audioMixer;
        // TODO: split these into groups, e.g. bgm, sound fx
        types::vector_arena<MIX_Track*> _availableAudioTracks;
        types::deque_arena<PlayingSound> _playingSounds;

        void stop(PlayingSound& playingSound);
    };

    /** Global Methods **/
    inline void setGlobalVolume(float volumeMultiplier) {
        slurp::Globals->AudioPlayer->setGlobalVolume(volumeMultiplier);
    }

    inline sound_id play(
        const asset::Sound* sound,
        float volumeMultiplier = 1.f,
        bool shouldLoop = false,
        const std::function<void()>& onFinish = [] {}
    ) {
        return slurp::Globals->AudioPlayer->play(sound, volumeMultiplier, shouldLoop, onFinish);
    }

    inline void stop(sound_id id) {
        slurp::Globals->AudioPlayer->stop(id);
    }

    inline void clearAll() {
        slurp::Globals->AudioPlayer->clearAll();
    }
}
