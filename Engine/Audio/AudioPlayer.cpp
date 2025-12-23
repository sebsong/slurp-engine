#include "AudioPlayer.h"

#include "Asset.h"
#include "PlayingSound.h"

#define NUM_AUDIO_TRACKS 128

namespace audio {
    static void onTrackFinish(void* userdata, MIX_Track* audioTrack) {
        AudioPlayer* audioPlayer = static_cast<AudioPlayer*>(userdata);
        if (audioPlayer) {
            audioPlayer->stop(audioTrack);
        }
    }

    AudioPlayer::AudioPlayer(MIX_Mixer* audioMixer)
        : _nextSoundId(1),
          _globalVolumeMultiplier(1.f),
          _audioMixer(audioMixer),
          _availableAudioTracks(types::vector_arena<MIX_Track*>()),
          _playingSounds(types::deque_arena<PlayingSound>()) {
        for (int i = 0; i < NUM_AUDIO_TRACKS; i++) {
            MIX_Track* audioTrack = MIX_CreateTrack(_audioMixer);
            if (!audioTrack) {
                ASSERT_LOG(false, "Failed to create MIX_Track.");
            }
            _availableAudioTracks.push_back(audioTrack);
        }
    }

    void AudioPlayer::setGlobalVolume(float volumeMultiplier) {
        _globalVolumeMultiplier = volumeMultiplier;
        MIX_SetMasterGain(_audioMixer, volumeMultiplier);
    }

    sound_id AudioPlayer::play(
        const asset::Sound* sound,
        float volumeMultiplier,
        bool shouldLoop,
        const std::function<void()>& onFinish
    ) {
        ASSERT(sound);
        if (!sound) {
            return INVALID_SOUND_ID;
        }

        MIX_Track* audioTrack;
        if (_availableAudioTracks.size() > 0) {
            audioTrack = _availableAudioTracks.back();
            _availableAudioTracks.pop_back();
        } else {
            ASSERT_LOG(_playingSounds.size() > 0, "No playing sounds, no available audio tracks.");
            PlayingSound oldestPlayingSound = _playingSounds.front();
            stop(oldestPlayingSound.id);
            audioTrack = oldestPlayingSound.audioTrack;
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
        _playingSounds.emplace_back(playingSound);
        return playingSound.id;
    }

    void AudioPlayer::stop(sound_id id) {
        for (auto it = _playingSounds.begin(); it != _playingSounds.end(); it++) {
            if (it->id == id) {
                stop(*it);
                _playingSounds.erase(it);
                return;
            }
        }
    }

    void AudioPlayer::stop(MIX_Track* audioTrack) {
        for (auto it = _playingSounds.begin(); it != _playingSounds.end(); it++) {
            if (it->audioTrack == audioTrack) {
                stop(*it);
                _playingSounds.erase(it);
                return;
            }
        }
    }

    void AudioPlayer::clearAll() {
        for (auto it = _playingSounds.begin(); it != _playingSounds.end(); it++) {
            it->isStopped = true;
            _availableAudioTracks.push_back(it->audioTrack);
        }
        _playingSounds.clear();
        MIX_StopAllTracks(_audioMixer, 0);
    }

    void AudioPlayer::stop(PlayingSound& playingSound) {
        if (playingSound.isStopped) {
            return;
        }
        playingSound.isStopped = true;
        MIX_StopTrack(playingSound.audioTrack, 0);
        _availableAudioTracks.push_back(playingSound.audioTrack);
        playingSound.onFinish();
    }
}
