#include "AudioPlayer.h"

#include "Asset.h"
#include "PlayingSound.h"
#include <SDL3_mixer/SDL_mixer.h>

namespace audio {
    static void initializeSoundGroup(
        MIX_Mixer* audioMixer,
        types::unordered_map_arena<sound_group_id, SoundGroupState>& soundGroupStates,
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
            SDL_SetFloatProperty(props, MIX_PROP_GROUP_GAIN_FLOAT, 1.f);
            if (!audioTrack) {
                ASSERT_LOG(false, "Failed to create MIX_Track.");
            }
            availableAudioTracks.push_back(audioTrack);
        }
    }

    AudioPlayer::AudioPlayer(MIX_Mixer* audioMixer)
        : _audioMixer(audioMixer),
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
            _soundGroupStates.contains(groupId),
            std::format("Could not find sound group id: {}", groupId)
        );
        if (!_soundGroupStates.contains(groupId)) {
            return;
        }

        MIX_Group* audioMixGroup = _soundGroupStates[groupId].audioMixGroup;
        SDL_PropertiesID props = MIX_GetGroupProperties(audioMixGroup);
        SDL_SetFloatProperty(props, MIX_PROP_GROUP_GAIN_FLOAT, volumeMultiplier);
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
            _soundGroupStates.contains(sound->groupId),
            std::format("Could not find sound group id: {}", sound->groupId)
        );
        if (!sound || !_soundGroupStates.contains(sound->groupId)) {
            return nullptr;
        }

        MIX_Group* audioMixGroup = _soundGroupStates[sound->groupId].audioMixGroup;
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
        SDL_PropertiesID groupProps = MIX_GetGroupProperties(audioMixGroup);
        float groupGain = SDL_GetFloatProperty(groupProps, MIX_PROP_GROUP_GAIN_FLOAT, 1.f);
        MIX_SetTrackGain(audioTrack, volumeMultiplier * groupGain);
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

        PlayingSound playingSound(sound, onFinish, audioTrack);
        return &playingSounds.emplace_back(playingSound);
    }

    void AudioPlayer::stop(PlayingSound* playingSound) {
        if (playingSound->isStopped) {
            return;
        }

        _stop(playingSound);
        sound_group_id groupId = playingSound->sound->groupId;
        types::deque_arena<PlayingSound>& playingSounds = _soundGroupStates[groupId].playingSounds;
        for (auto it = playingSounds.begin(); it != playingSounds.end(); it++) {
            if (&*it == playingSound) {
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
                        _stop(&*it);
                        playingSounds.erase(it);
                        return;
                    }
                }
                return;
            }
        }
        ASSERT_LOG(false, "Could not find audio track.");
    }

    void AudioPlayer::pause(PlayingSound* playingSound) {
        if (playingSound->isPaused) {
            return;
        }
        playingSound->isPaused = true;
        MIX_PauseTrack(playingSound->audioTrack);
    }

    void AudioPlayer::resume(PlayingSound* playingSound) {
        if (!playingSound->isPaused) {
            return;
        }
        playingSound->isPaused = false;
        MIX_ResumeTrack(playingSound->audioTrack);
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

    void AudioPlayer::_stop(PlayingSound* playingSound) {
        if (playingSound->isStopped) {
            return;
        }

        sound_group_id groupId = playingSound->sound->groupId;
        MIX_StopTrack(playingSound->audioTrack, 0);
        types::vector_arena<MIX_Track*>& availableAudioTracks = _soundGroupStates[groupId].availableAudioTracks;
        availableAudioTracks.push_back(playingSound->audioTrack);

        playingSound->isStopped = true;
        playingSound->onFinish();
    }
}
