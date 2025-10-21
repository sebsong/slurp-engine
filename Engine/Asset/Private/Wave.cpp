#include "Wave.h"

#include <mmeapi.h>

namespace asset {
    static audio::channel_audio_sample_container_t getChannelSample(
        const types::byte* chunkData,
        uint32_t totalSampleSize,
        uint32_t sampleIdx,
        uint32_t numChannels,
        uint32_t channelIdx
    ) {
        audio::channel_audio_sample_container_t sample = 0;
        // TODO: move some of these outside
        uint32_t perChannelSampleSizeBytes = totalSampleSize / numChannels;
        uint32_t byteOffset = (sampleIdx * totalSampleSize) +
                              (channelIdx * perChannelSampleSizeBytes);
        std::copy_n(
            chunkData + byteOffset,
            perChannelSampleSizeBytes,
            reinterpret_cast<types::byte*>(&sample)
        );
        return bit_twiddle::upsizeInt(
            sample,
            perChannelSampleSizeBytes,
            sizeof(sample)
        );
    }

    void loadWaveData(Sound* sound, const types::byte* waveFileBytes, uint32_t waveFileSizeBytes) {
        ASSERT(IS_STEREO_AUDIO); // NOTE: assumes output is always stereo

        const types::byte* chunkData = waveFileBytes;
        const FormatChunk* formatChunk = nullptr;
        while (chunkData < waveFileBytes + waveFileSizeBytes) {
            const WaveChunk* chunk = reinterpret_cast<const WaveChunk*>(chunkData);
            switch (chunk->chunkId) {
                case (Riff): {
                    const RiffChunk* riffChunk = reinterpret_cast<const RiffChunk*>(chunkData);
                    ASSERT(riffChunk->waveId == Wave);
                    chunkData = riffChunk->chunkData;
                    continue;
                }
                break;
                case (Format): {
                    formatChunk = reinterpret_cast<const FormatChunk*>(chunkData);
                    // NOTE: coupled with platform audio buffer settings
                    ASSERT(formatChunk->formatTag == WAVE_FORMAT_PCM);
                    ASSERT(formatChunk->numChannels <= STEREO_NUM_AUDIO_CHANNELS);
                    ASSERT(formatChunk->sampleSizeBytes <= TOTAL_AUDIO_SAMPLE_SIZE);
                    ASSERT(
                        (formatChunk->sampleSizeBytes / formatChunk->numChannels) <=
                        (PER_CHANNEL_AUDIO_SAMPLE_SIZE)
                    );
                }
                break;
                case (Data): {
                    ASSERT(formatChunk);
                    chunkData = chunk->chunkData;
                    uint32_t numSamples = chunk->chunkSizeBytes / formatChunk->sampleSizeBytes;
                    audio::StereoAudioSampleContainer* sampleData =
                            slurp::Globals->GameMemory->permanent->allocate<audio::StereoAudioSampleContainer>(numSamples);

                    uint64_t volumeMultiplier =
                            bit_twiddle::maxSignedValue(PER_CHANNEL_AUDIO_SAMPLE_SIZE) /
                            bit_twiddle::maxSignedValue(
                                formatChunk->sampleSizeBytes / formatChunk->numChannels
                            );
                    if (formatChunk->numChannels == MONO_NUM_AUDIO_CHANNELS) {
                        for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
                            audio::channel_audio_sample_container_t sample =
                                    getChannelSample(
                                        chunkData,
                                        formatChunk->sampleSizeBytes,
                                        sampleIdx,
                                        formatChunk->numChannels,
                                        MONO_AUDIO_CHANNEL_IDX
                                    ) * volumeMultiplier;
                            sampleData[sampleIdx] = audio::StereoAudioSampleContainer{sample, sample};
                        }
                    } else if (formatChunk->numChannels == STEREO_NUM_AUDIO_CHANNELS) {
                        for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
                            audio::channel_audio_sample_container_t leftSample = getChannelSample(
                                    chunkData,
                                    formatChunk->sampleSizeBytes,
                                    sampleIdx,
                                    formatChunk->numChannels,
                                    STEREO_LEFT_AUDIO_CHANNEL_IDX
                                ) * volumeMultiplier;

                            audio::channel_audio_sample_container_t rightSample = getChannelSample(
                                    chunkData,
                                    formatChunk->sampleSizeBytes,
                                    sampleIdx,
                                    formatChunk->numChannels,
                                    STEREO_RIGHT_AUDIO_CHANNEL_IDX
                                ) * volumeMultiplier;

                            sampleData[sampleIdx] = audio::StereoAudioSampleContainer{leftSample, rightSample};
                        }
                    } else {
                        ASSERT(false);
                    }
                    sound->numSamples = numSamples;
                    sound->sampleData = sampleData;
                    sound->isLoaded = true;
                    return;
                }
                case (Bext):
                case (Junk):
                case (JUNK):
                    break;
                default: {
                    ASSERT(false);
                }
            }

            chunkData = chunk->chunkData + chunk->chunkSizeBytes;
        }
    }
}
