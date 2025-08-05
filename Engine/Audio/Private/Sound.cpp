#include "Sound.h"

#include "Asset.h"
#include "Audio.h"

namespace audio {
    void Sound::loadAudio(const AudioBuffer& buffer) {
        int numSamplesWritten = 0;
        while (numSamplesWritten < buffer.numSamplesToWrite && sampleIndex < numSamples) {
            buffer.samples[numSamplesWritten++] = sampleData[sampleIndex++];
        }
    }

    Sound loadSound(const std::string& waveFileName) {
        asset::WaveData waveData = asset::loadWaveFile(waveFileName);
        return Sound{
            0,
            waveData.numSamples,
            waveData.sampleData
        };
    }
}
