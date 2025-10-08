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

    void PlayingSound::bufferAudio(const AudioBuffer& buffer, float globalVolumeMultiplier) {
        int numSamplesWritten = 0;
        while (numSamplesWritten < buffer.numSamplesToWrite && sampleIndex < sound->numSamples) {
            // TODO: allow for pitch shifting
            // TODO: do full mixing in 32-bit space to avoid clipping, convert back to 16-bit space just before writing to buffer
            // NOTE: we could pre-process volume multiplier to trade memory for speed
            float volumeMultiplier = this->volumeMultiplier * globalVolumeMultiplier;
            const StereoAudioSampleContainer& fullSample = sound->sampleData[sampleIndex++];
            buffer.samples[numSamplesWritten++] += StereoAudioSample{
                static_cast<channel_audio_sample_t>(fullSample.left * volumeMultiplier),
                static_cast<channel_audio_sample_t>(fullSample.right * volumeMultiplier)
            };
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
