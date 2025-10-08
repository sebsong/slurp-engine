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
    }

    void SoundManager::bufferAudio(const AudioBuffer& buffer) {
        std::fill_n(buffer.samples, buffer.numSamplesToWrite, static_cast<audio_sample_t>(0));

        for (std::deque<PlayingSound>::iterator it = _queue.begin(); it != _queue.end();) {
            PlayingSound& playingSound = *it;

            playingSound.bufferAudio(buffer, _globalVolumeMultiplier);

            if (!playingSound.isPlaying) {
                it = _queue.erase(it);
            } else {
                it++;
            }
        }
    }
}
