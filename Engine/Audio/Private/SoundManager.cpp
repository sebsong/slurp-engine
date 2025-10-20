#include "SoundManager.h"

#include "PlayingSound.h"

namespace audio {
    SoundManager::SoundManager(): _nextSoundId(0),
                                  _globalVolumeMultiplier(1.f),
                                  _loopingQueue(std::deque<PlayingSound>()),
                                  _oneShotQueue(std::deque<PlayingSound>()) {}

    void SoundManager::setGlobalVolume(float volumeMultiplier) {
        _globalVolumeMultiplier = volumeMultiplier;
    }

    void SoundManager::playSound(const asset::Sound* sound) {
        playSound(sound, 1.0f, false);
    }

    void SoundManager::playSound(const asset::Sound* sound, float volumeMultiplier, bool shouldLoop) {
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
        std::deque<PlayingSound>& queue,
        int numSamplesToWrite,
        float volumeMultiplier,
        bool dampMix
    ) {
        for (std::deque<PlayingSound>::iterator it = queue.begin(); it != queue.end();) {
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

    void SoundManager::bufferAudio(const AudioBuffer& buffer) {
        // TODO: allocate from custom memory allocator instead
        StereoAudioSampleContainer* sampleContainers = new StereoAudioSampleContainer[buffer.numSamplesToWrite]();

        /** Buffer one shot sounds first with damping to avoid clipping **/
        /** Buffer looping sounds after without damping to preserve persistent sounds (e.g. music) **/
        bufferFromQueue(sampleContainers, _oneShotQueue, buffer.numSamplesToWrite, _globalVolumeMultiplier, true);
        bufferFromQueue(sampleContainers, _loopingQueue, buffer.numSamplesToWrite, _globalVolumeMultiplier, false);

        std::copy_n(sampleContainers, buffer.numSamplesToWrite, buffer.samples);
        delete[] sampleContainers;
    }
}
