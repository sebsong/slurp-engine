#include "AudioPlayer.h"

#include "Asset.h"
#include "PlayingSound.h"
#include <SDL3_mixer/SDL_mixer.h>

namespace audio {
    static void initializeSoundGroup(
        MIX_Mixer* audioMixer,
        types::unordered_map_arena<sound_group_id, SoundGroupState> soundGroupStates,
        sound_group_id groupId,
        int numTracks
    ) {
        MIX_Group* audioMixGroup = MIX_CreateGroup(audioMixer);
        soundGroupStates[groupId] = SoundGroupState{
            audioMixGroup,
            types::vector_arena<MIX_Track*>(),
            types::deque_arena<PlayingSound>()
        };
        types::vector_arena<MIX_Track*>& availableAudioTracks = soundGroupStates[groupId].availableAudioTracks;
        availableAudioTracks.reserve(numTracks);
        for (int i = 0; i < numTracks; i++) {
            MIX_Track* audioTrack = MIX_CreateTrack(audioMixer);
            MIX_SetTrackGroup(audioTrack, audioMixGroup);
            SDL_PropertiesID props = MIX_GetTrackProperties(audioTrack);
            SDL_SetNumberProperty(props, MIX_PROP_GROUP_ID_NUMBER, groupId);
            if (!audioTrack) {
                ASSERT_LOG(false, "Failed to create MIX_Track.");
            }
            availableAudioTracks.push_back(audioTrack);
        }
    }

    AudioPlayer::AudioPlayer(MIX_Mixer* audioMixer)
        : _nextSoundId(1),
          _audioMixer(audioMixer),
          _soundGroupStates(types::unordered_map_arena<sound_group_id, SoundGroupState>()) {
        initializeSoundGroup(_audioMixer, _soundGroupStates, AUDIO_SOUND_GROUP_BGM, NUM_AUDIO_TRACKS_BGM);
        initializeSoundGroup(_audioMixer, _soundGroupStates, AUDIO_SOUND_GROUP_SFX, NUM_AUDIO_TRACKS_SFX);
        initializeSoundGroup(_audioMixer, _soundGroupStates, AUDIO_SOUND_GROUP_OTHER, NUM_AUDIO_TRACKS_OTHER);
    }

    void AudioPlayer::setGlobalVolume(float volumeMultiplier) const {
        MIX_SetMasterGain(_audioMixer, volumeMultiplier);
    }

    void AudioPlayer::setGroupVolume(sound_group_id groupId, float volumeMultiplier) {
        ASSERT_LOG(
            !_soundGroupStates.contains(groupId),
            std::format("Could not find sound group id: {}", groupId)
        );
        if (!_soundGroupStates.contains(groupId)) {
            return;
        }

        MIX_Group* audioMixGroup = _soundGroupStates[groupId].audioMixGroup;
        // TODO: read this out and use it later, move name to constant
        SDL_PropertiesID props = MIX_GetGroupProperties(audioMixGroup);
        SDL_SetFloatProperty(props, "GroupGain", volumeMultiplier);
    }

    static void onTrackFinish(void* userdata, MIX_Track* audioTrack) {
        AudioPlayer* audioPlayer = static_cast<AudioPlayer*>(userdata);
        if (audioPlayer) {
            audioPlayer->stop(audioTrack);
        }
    }

    PlayingSound* AudioPlayer::play(
        const asset::Sound* sound,
        float volumeMultiplier,
        bool shouldLoop,
        const std::function<void()>& onFinish
    ) {
        ASSERT_LOG(sound, "Sound not provided.");
        ASSERT_LOG(
            !_soundGroupStates.contains(sound->groupId),
            std::format("Could not find sound group id: {}", sound->groupId)
        );
        if (!sound || !_soundGroupStates.contains(sound->groupId)) {
            return nullptr;
        }

        types::vector_arena<MIX_Track*>& availableAudioTracks = _soundGroupStates[sound->groupId].availableAudioTracks;
        types::deque_arena<PlayingSound>& playingSounds = _soundGroupStates[sound->groupId].playingSounds;

        MIX_Track* audioTrack;
        if (!availableAudioTracks.empty()) {
            audioTrack = availableAudioTracks.back();
            availableAudioTracks.pop_back();
        } else {
            ASSERT_LOG(!playingSounds.empty(), "No playing sounds, no available audio tracks.");
            PlayingSound* oldestPlayingSound = &playingSounds.front();
            stop(oldestPlayingSound);
            audioTrack = oldestPlayingSound->audioTrack;
        }
        MIX_SetTrackAudio(audioTrack, sound->audio);
        MIX_SetTrackGain(audioTrack, volumeMultiplier);
        SDL_PropertiesID propertiesId = SDL_CreateProperties();
        if (shouldLoop) {
            SDL_SetNumberProperty(propertiesId, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        }
        MIX_SetTrackStoppedCallback(
            audioTrack,
            &onTrackFinish,
            this
        );
        MIX_PlayTrack(audioTrack, propertiesId);

        PlayingSound playingSound(_nextSoundId++, sound, onFinish, audioTrack);
        return &playingSounds.emplace_back(playingSound);
    }

    void AudioPlayer::stop(PlayingSound* playingSound) {
        sound_group_id groupId = playingSound->sound->groupId;
        types::deque_arena<PlayingSound>& playingSounds = _soundGroupStates[groupId].playingSounds;
        for (auto it = playingSounds.begin(); it != playingSounds.end(); it++) {
            if (it->id == playingSound->id) {
                _stop(*it);
                playingSounds.erase(it);
                return;
            }
        }
    }

    void AudioPlayer::stop(MIX_Track* audioTrack) {
        for (auto& [groupId, soundGroupState]: _soundGroupStates) {
            SDL_PropertiesID props = MIX_GetTrackProperties(audioTrack);
            sound_group_id trackGroupId = SDL_GetNumberProperty(props, MIX_PROP_GROUP_ID_NUMBER, -1);
            if (trackGroupId == groupId) {
                types::deque_arena<PlayingSound>& playingSounds = soundGroupState.playingSounds;
                for (auto it = playingSounds.begin(); it != playingSounds.end(); it++) {
                    if (it->audioTrack == audioTrack) {
                        _stop(*it);
                        playingSounds.erase(it);
                        return;
                    }
                }
                return;
            }
        }
        ASSERT_LOG(false, "Could not find audio track.");
    }

    void AudioPlayer::clearAll() {
        for (auto& [_, soundGroupState]: _soundGroupStates) {
            for (PlayingSound& _playingSound: soundGroupState.playingSounds) {
                _playingSound.isStopped = true;
                soundGroupState.availableAudioTracks.push_back(_playingSound.audioTrack);
            }
            soundGroupState.playingSounds.clear();
        }
        MIX_StopAllTracks(_audioMixer, 0);
    }

    void AudioPlayer::_stop(PlayingSound& playingSound) {
        sound_group_id groupId = playingSound.sound->groupId;
        types::vector_arena<MIX_Track*>& availableAudioTracks = _soundGroupStates[groupId].availableAudioTracks;
        if (playingSound.isStopped) {
            return;
        }
        playingSound.isStopped = true;
        MIX_StopTrack(playingSound.audioTrack, 0);
        availableAudioTracks.push_back(playingSound.audioTrack);
        playingSound.onFinish();
    }
}
