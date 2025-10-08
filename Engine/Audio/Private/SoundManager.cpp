#include "SoundManager.h"

#include "Sound.h"

namespace audio {
    SoundManager::SoundManager(): _nextSoundId(0), _globalVolumeMultiplier(1.f), _queue(std::deque<PlayingSound>()) {}

    void SoundManager::setGlobalVolume(float volumeMultiplier) {
        _globalVolumeMultiplier = volumeMultiplier;
    }

    void SoundManager::playSound(const Sound& sound) {
        playSound(sound, 1.0f, false);
    }

    void SoundManager::playSound(const Sound& sound, float volumeMultiplier, bool shouldLoop) {
        ASSERT(sound.sampleData);
        if (!sound.sampleData) {
            return;
        }
        _queue.push_back(PlayingSound(_nextSoundId++, &sound, volumeMultiplier, shouldLoop));

        // TODO: have separate maximums for different sound categories
        if (_queue.size() > MAX_NUM_PLAYING_SOUNDS) {
            _queue.pop_front();
        }
    }

    void SoundManager::bufferAudio(const AudioBuffer& buffer) {
        // TODO: allocate from custom memory allocator instead
        StereoAudioSampleContainer* sampleContainers = new StereoAudioSampleContainer[buffer.numSamplesToWrite]();

        for (std::deque<PlayingSound>::iterator it = _queue.begin(); it != _queue.end();) {
            PlayingSound& playingSound = *it;

            playingSound.bufferAudio(sampleContainers, buffer.numSamplesToWrite, _globalVolumeMultiplier);

            if (!playingSound.isPlaying) {
                it = _queue.erase(it);
            } else {
                it++;
            }
        }

        std::copy_n(sampleContainers, buffer.numSamplesToWrite, buffer.samples);
        delete[] sampleContainers;
    }
}
