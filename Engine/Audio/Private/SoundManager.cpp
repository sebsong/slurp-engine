#include "SoundManager.h"

#include "Sound.h"

namespace audio {
    SoundManager::SoundManager(): _nextSoundId(0), _queue(std::deque<PlayingSound>()) {}

    void SoundManager::play(const Sound& sound, float volumeMultiplier, bool shouldLoop) {
        _queue.push_back(PlayingSound(_nextSoundId++, &sound, volumeMultiplier, shouldLoop));
    }

    void SoundManager::loadAudio(const AudioBuffer& buffer) {
        std::fill_n(buffer.samples, buffer.numSamplesToWrite, 0);

        for (std::deque<PlayingSound>::iterator it = _queue.begin(); it != _queue.end();) {
            PlayingSound& playingSound = *it;
            playingSound.loadAudio(buffer);
            if (!playingSound.isPlaying) {
                it = _queue.erase(it);
            } else {
                it++;
            }
        }
    }
}
