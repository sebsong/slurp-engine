#include "Sound.h"

#include "Asset.h"
#include "Audio.h"

namespace audio {
    void Sound::loadAudio(const AudioBuffer& buffer) const {
        memcpy(buffer.samples, waveData.sampleData, waveData.numBytes);
    }

    Sound loadSound(const std::string& waveFileName) {
        return Sound{
            asset::loadWaveFile(waveFileName)
        };
    }
}
