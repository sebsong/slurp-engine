#include "Sound.h"

#include "Asset.h"
#include "Audio.h"

namespace audio {
    PlayingSound::PlayingSound(uint32_t id, const Sound* sound, float volumeMultiplier, bool shouldLoop)
        : id(id),
          sound(sound),
          volumeMultiplier(volumeMultiplier),
          shouldLoop(shouldLoop),
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
        }
    }

    bool PlayingSound::operator==(const PlayingSound& other) const {
        return id == other.id;
    }

    Sound loadSound(const std::string& waveFileName) {
        asset::WaveData waveData = asset::loadWaveFile(waveFileName);
        return Sound{
            waveData
        };
    }
}
