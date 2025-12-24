#pragma once

#include "CollectionTypes.h"
#include "PlayingSound.h"

struct MIX_Mixer;
struct MIX_Group;
struct MIX_Track;

#define MIX_PROP_GROUP_ID_NUMBER "SDL_mixer.group.id"
#define AUDIO_SOUND_GROUP_BGM 1
#define AUDIO_SOUND_GROUP_SFX 2
#define AUDIO_SOUND_GROUP_OTHER 3

#define NUM_AUDIO_TRACKS_BGM 8
#define NUM_AUDIO_TRACKS_SFX 64
#define NUM_AUDIO_TRACKS_OTHER 64

namespace audio {
    struct AudioBuffer;

    typedef uint32_t sound_id;
    static constexpr sound_id INVALID_SOUND_ID = 0;

    typedef uint32_t sound_group_id;

    struct SoundGroupState {
        MIX_Group* audioMixGroup;
        types::vector_arena<MIX_Track*> availableAudioTracks;
        types::deque_arena<PlayingSound> playingSounds;
    };

    class AudioPlayer {
    public:
        explicit AudioPlayer(MIX_Mixer* audioMixer);

        void setGlobalVolume(float volumeMultiplier) const;

        void setGroupVolume(sound_group_id groupId, float volumeMultiplier);

        PlayingSound* play(
            const asset::Sound* sound,
            float volumeMultiplier,
            bool shouldLoop,
            const std::function<void()>& onFinish
        );

        void stop(PlayingSound* playingSound);

        void stop(MIX_Track* audioTrack);

        void clearAll();

    private:
        sound_id _nextSoundId;
        MIX_Mixer* _audioMixer;
        types::unordered_map_arena<sound_group_id, SoundGroupState> _soundGroupStates;

        void _stop(PlayingSound& playingSound);
    };

    /** Global Methods **/
    inline void setGlobalVolume(float volumeMultiplier) {
        slurp::Globals->AudioPlayer->setGlobalVolume(volumeMultiplier);
    }

    inline PlayingSound* play(
        const asset::Sound* sound,
        float volumeMultiplier = 1.f,
        bool shouldLoop = false,
        const std::function<void()>& onFinish = [] {}
    ) {
        return slurp::Globals->AudioPlayer->play(sound, volumeMultiplier, shouldLoop, onFinish);
    }

    inline void stop(PlayingSound* playingSound) {
        slurp::Globals->AudioPlayer->stop(playingSound);
    }

    inline void clearAll() {
        slurp::Globals->AudioPlayer->clearAll();
    }
}
