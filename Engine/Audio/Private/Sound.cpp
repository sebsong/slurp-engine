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
        float globalVolumeMultiplier
    ) {
        int numSamplesWritten = 0;
        while (numSamplesWritten < numSamplesToWrite && sampleIndex < sound->numSamples) {
            // TODO: allow for pitch shifting
            float volumeMultiplier = this->volumeMultiplier * globalVolumeMultiplier;
            sampleContainers[numSamplesWritten++] += sound->sampleData[sampleIndex++] * volumeMultiplier;
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
            waveData.numSamples,
            waveData.sampleData
        };
    }
}
