#include "AudioPlayer.h"

#include "PlayingSound.h"

namespace audio {
    AudioPlayer::AudioPlayer(): _nextSoundId(0),
                                _globalVolumeMultiplier(1.f),
                                _loopingQueue(types::deque_arena<PlayingSound>()),
                                _oneShotQueue(types::deque_arena<PlayingSound>()) {}

    void AudioPlayer::setGlobalVolume(float volumeMultiplier) {
        _globalVolumeMultiplier = volumeMultiplier;
    }

    void AudioPlayer::play(const asset::Sound* sound) {
        play(sound, 1.0f, false);
    }

    void AudioPlayer::play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop) {
        ASSERT(sound);
        if (!sound) {
            return;
        }

        PlayingSound playingSound(_nextSoundId++, sound, volumeMultiplier, shouldLoop);
        if (shouldLoop) {
            _loopingQueue.push_back(playingSound);
        } else {
            _oneShotQueue.push_back(playingSound);
        }

        // TODO: have separate maximums for different sound categories
        if (_oneShotQueue.size() > MAX_NUM_PLAYING_ONE_SHOT_SOUNDS) {
            _oneShotQueue.pop_front();
        }
    }

    static void bufferFromQueue(
        StereoAudioSampleContainer* sampleContainers,
        types::deque_arena<PlayingSound>& queue,
        int numSamplesToWrite,
        float volumeMultiplier,
        bool dampMix
    ) {
        for (types::deque_arena<PlayingSound>::iterator it = queue.begin(); it != queue.end();) {
            PlayingSound& playingSound = *it;
            if (!playingSound.sound->isLoaded) {
                it++;
                continue;
            }

            playingSound.bufferAudio(sampleContainers, numSamplesToWrite, volumeMultiplier, dampMix);

            if (!playingSound.isPlaying) {
                it = queue.erase(it);
            } else {
                it++;
            }
        }
    }

    void AudioPlayer::bufferAudio(const AudioBuffer& buffer) {
        StereoAudioSampleContainer* sampleContainers =
                memory::SingleFrame->allocate<StereoAudioSampleContainer>(
                    buffer.numSamplesToWrite,
                    true
                );

        /** Buffer one shot sounds first with damping to avoid clipping **/
        /** Buffer looping sounds after without damping to preserve persistent sounds (e.g. music) **/
        bufferFromQueue(sampleContainers, _oneShotQueue, buffer.numSamplesToWrite, _globalVolumeMultiplier, true);
        bufferFromQueue(sampleContainers, _loopingQueue, buffer.numSamplesToWrite, _globalVolumeMultiplier, false);

        std::copy_n(sampleContainers, buffer.numSamplesToWrite, buffer.samples);
    }

    /** Global Methods **/

    void setGlobalVolume(float volumeMultiplier) {
        slurp::Globals->AudioManager->setGlobalVolume(volumeMultiplier);
    }

    void play(const asset::Sound* sound) {
        slurp::Globals->AudioManager->play(sound);
    }

    void play(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop) {
        slurp::Globals->AudioManager->play(sound, volumeMultiplier, shouldLoop);
    }

    void bufferAudio(const AudioBuffer& buffer) {
        slurp::Globals->AudioManager->bufferAudio(buffer);
    }
}
