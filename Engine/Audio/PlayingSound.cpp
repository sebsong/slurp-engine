#include "PlayingSound.h"

#include "Audio.h"
#include "Asset.h"

namespace audio {
    PlayingSound::PlayingSound(
        uint32_t id,
        const asset::Sound* sound,
        float volumeMultiplier,
        bool shouldLoop,
        std::function<void()> onFinish
    )
        : id(id),
          sound(sound),
          volumeMultiplier(volumeMultiplier),
          shouldLoop(shouldLoop),
          onFinish(onFinish),
          sampleIndex(0),
          isPlaying(true) {}

    void PlayingSound::bufferAudio(
        StereoAudioSampleContainer* sampleContainers,
        int numSamplesToWrite,
        float globalVolumeMultiplier,
        bool dampMix
    ) {
        int numSamplesWritten = 0;
        while (numSamplesWritten < numSamplesToWrite && sampleIndex < sound->numSamples) {
            // TODO: allow for pitch shifting
            float volumeMultiplier = this->volumeMultiplier * globalVolumeMultiplier;
            StereoAudioSampleContainer& sampleContainer = sampleContainers[numSamplesWritten++];
            const StereoAudioSampleContainer& sample = sound->sampleData[sampleIndex++] * volumeMultiplier;
            sampleContainer += sample;
            if (dampMix) {
                sampleContainer *= SOUND_DAMP_FACTOR;
            }
        }

        if (sampleIndex >= sound->numSamples) {
            if (shouldLoop) {
                sampleIndex = 0;
            } else {
                isPlaying = false;
            }
            onFinish();
        }
    }

    bool PlayingSound::operator==(const PlayingSound& other) const {
        return id == other.id;
    }
}
