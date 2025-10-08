#include "Asset.h"

#include "BitTwiddle.h"
#include "Debug.h"
#include "Types.h"
#include "WinEngine.h"

#include <filesystem>
#include <fstream>

namespace asset {
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";
    static const std::string SpritesDirectory = AssetsDirectory + "Sprites/";
    static const std::string SoundsDirectory = AssetsDirectory + "Sounds/";
    static const std::string ShadersDirectory = AssetsDirectory + "Shaders/";
    static const std::string VertexShadersDirectory = ShadersDirectory + "1_Vertex/";
    static const std::string FragmentShadersDirectory = ShadersDirectory + "2_Fragment/";

    static constexpr uint8_t FourBitMaskLow = 0b00001111;
    static constexpr uint8_t FourBitMaskHigh = 0b11110000;

    static std::ifstream getFile(const std::string& directory, const std::string& fileName) {
        const std::string filePath = directory + fileName;
        std::ifstream file(filePath, std::ios::binary);

        ASSERT(file.good());

        return file;
    }

    static FileReadResult readBytes(const std::string& directory, const std::string& fileName) {
        const std::string filePath = directory + fileName;
        std::ifstream file(filePath, std::ios::binary);

        ASSERT(file.good());
        if (!file.good()) { return {}; }

        uint32_t fileSizeBytes = std::filesystem::file_size(filePath);
        types::byte* fileBytes = new types::byte[fileSizeBytes]; // TODO: need to free this memory
        file.read(reinterpret_cast<std::istream::char_type*>(fileBytes), fileSizeBytes);

        return FileReadResult{
            fileSizeBytes,
            fileBytes
        };
    }

    static std::string readTextFile(const std::string& directory, const std::string& fileName) {
        const std::string filePath = directory + fileName;
        std::ifstream file(filePath);
        ASSERT(file.good());

        std::ostringstream stream;
        stream << file.rdbuf();

        return stream.str();
    }

    render::ColorPalette loadColorPalette(const std::string& paletteHexFileName) {
        render::ColorPalette palette = {};

        const std::string filePath = PalettesDirectory + paletteHexFileName;
        std::ifstream file(filePath);
        ASSERT(file.good());

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE) {
            render::Pixel color = std::stoi(line, nullptr, 16);
            if (colorPaletteIdx != 0) {
                color |= render::AlphaMask;
            }
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
    }

    struct FileBytesReadResult {
        uintmax_t numBytes;
        types::byte* bytes;
    };

    static void writePixel(
        int x,
        int y,
        int width,
        int height,
        render::Pixel color,
        render::Pixel* outMap,
        bool invertVertical
    ) {
        int yCoord = invertVertical ? (height - 1 - y) : y;
        outMap[x + yCoord * width] = color;
    }

    static void loadBitmapColorPalette(
        types::byte* spriteFileBytes,
        types::byte* bitmapBytes,
        int width,
        int height,
        int colorPaletteSize,
        int bitsPerIndex,
        render::Pixel* outMap,
        bool invertVertical = false
    ) {
        ASSERT(bitsPerIndex == 4);

        render::Pixel* colorPalette = reinterpret_cast<render::Pixel*>(spriteFileBytes + sizeof(BitmapHeader));
        for (int i = 0; i < colorPaletteSize; i++) {
            if (i == 0) {
                // NOTE: Assumes that the 0 index color is the transparent color
                colorPalette[i] = 0;
            } else {
                // NOTE: indexed colors don't support alpha channel (at least in aesprite)
                colorPalette[i] |= render::AlphaMask;
            }
        }

        int rowSizeBits = width * bitsPerIndex;
        int rowSizeBytes = rowSizeBits / 8 + (rowSizeBits % 8 != 0);
        int rowRemainderBytes = rowSizeBytes % 4;
        if (rowRemainderBytes != 0) {
            // NOTE: bitmaps are padded to 4 byte words
            rowSizeBytes += 4 - rowRemainderBytes;
        }
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // TODO: avoid an extra read by re-using this for low + high bitmasking
                types::byte colorIndex = bitmapBytes[x / 2 + y * rowSizeBytes];
                if (x % 2 == 0) { colorIndex = (colorIndex & FourBitMaskHigh) >> 4; } else {
                    colorIndex &= FourBitMaskLow;
                }
                render::Pixel color = colorPalette[colorIndex];
                writePixel(
                    x,
                    y,
                    width,
                    height,
                    color,
                    outMap,
                    invertVertical
                );
            }
        }
    }

    static void loadBitmapBitFields(
        types::byte* bitmapBytes,
        int width,
        int height,
        int bitsPerPixel,
        render::Pixel* outMap,
        bool invertVertical = false
    ) {
        ASSERT(bitsPerPixel == 32);
        render::Pixel* pixels = reinterpret_cast<render::Pixel*>(bitmapBytes);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                render::Pixel color = pixels[x + y * width];
                writePixel(
                    x,
                    y,
                    width,
                    height,
                    color,
                    outMap,
                    invertVertical
                );
            }
        }
    }

    // TODO: move this to windows layer?
    Bitmap loadBitmapFile(const std::string& bitmapFileName) {
        types::byte* fileBytes = readBytes(SpritesDirectory, bitmapFileName).contents;
        if (!fileBytes) {
            return Bitmap{};
        }

        BitmapHeader* header = reinterpret_cast<BitmapHeader*>(fileBytes);

        types::byte* bitmapBytes = fileBytes + header->fileHeader.bfOffBits;
        int width = static_cast<int>(header->infoHeader.biWidth);
        int height = static_cast<int>(header->infoHeader.biHeight);
        render::Pixel* map = new render::Pixel[width * height];
        if (header->infoHeader.biCompression == BI_RGB && header->infoHeader.biBitCount <= 8) {
            loadBitmapColorPalette(
                fileBytes,
                bitmapBytes,
                width,
                height,
                static_cast<int>(header->infoHeader.biClrUsed),
                header->infoHeader.biBitCount,
                map
            );
        } else if (header->infoHeader.biCompression == BI_BITFIELDS) {
            loadBitmapBitFields(
                bitmapBytes,
                width,
                height,
                header->infoHeader.biBitCount,
                map
            );
        } else {
            // TODO: unsupported compression type
            ASSERT(false);
        }

        return Bitmap{
            {width, height},
            map
        };
    }

    std::string loadVertexShaderSource(const std::string& shaderSourceFileName) {
        return readTextFile(VertexShadersDirectory, shaderSourceFileName);
    }

    std::string loadFragmentShaderSource(const std::string& shaderSourceFileName) {
        return readTextFile(FragmentShadersDirectory, shaderSourceFileName);
    }

    static audio::channel_audio_sample_container_t getChannelSample(
        types::byte* chunkData,
        uint32_t totalSampleSize,
        uint32_t sampleIdx,
        uint32_t numChannels,
        uint32_t channelIdx,
        uint64_t volumeMultiplier
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
        sample = bit_twiddle::upsizeInt(
            sample,
            perChannelSampleSizeBytes,
            PER_CHANNEL_AUDIO_SAMPLE_SIZE
        );
        return audio::modulateSampleVolume(sample, volumeMultiplier);
    }

    // TODO: pre-process wave files into the engine sample size
    // TODO: stream the file in async
    WaveData loadWaveFile(const std::string& waveFileName) {
        FileReadResult fileReadResult = readBytes(SoundsDirectory, waveFileName);
        types::byte* fileBytes = fileReadResult.contents; // TODO: free the memory
        if (!fileBytes) {
            return WaveData{};
        }

        ASSERT(IS_STEREO_AUDIO); // NOTE: assumes output is always stereo

        types::byte* chunkData = fileBytes;
        FormatChunk* formatChunk = nullptr;
        while (chunkData < fileBytes + fileReadResult.sizeBytes) {
            WaveChunk* chunk = reinterpret_cast<WaveChunk*>(chunkData);
            switch (chunk->chunkId) {
                case (Riff): {
                    RiffChunk* riffChunk = reinterpret_cast<RiffChunk*>(chunkData);
                    ASSERT(riffChunk->waveId == Wave);
                    chunkData = riffChunk->chunkData;
                    continue;
                }
                break;
                case (Format): {
                    formatChunk = reinterpret_cast<FormatChunk*>(chunkData);
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
                    audio::StereoAudioSampleContainer* sampleData = new audio::StereoAudioSampleContainer[numSamples];

                    uint64_t volumeMultiplier = bit_twiddle::maxSignedValue(PER_CHANNEL_AUDIO_SAMPLE_SIZE) /
                                                bit_twiddle::maxSignedValue(
                                                    formatChunk->sampleSizeBytes / formatChunk->numChannels
                                                );
                    if (formatChunk->numChannels == MONO_NUM_AUDIO_CHANNELS) {
                        for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
                            audio::channel_audio_sample_container_t sample = getChannelSample(
                                chunkData,
                                formatChunk->sampleSizeBytes,
                                sampleIdx,
                                formatChunk->numChannels,
                                MONO_AUDIO_CHANNEL_IDX,
                                volumeMultiplier
                            );

                            sampleData[sampleIdx] = audio::StereoAudioSampleContainer{sample, sample};
                        }
                    } else if (formatChunk->numChannels == STEREO_NUM_AUDIO_CHANNELS) {
                        for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
                            audio::channel_audio_sample_container_t leftSample = getChannelSample(
                                chunkData,
                                formatChunk->sampleSizeBytes,
                                sampleIdx,
                                formatChunk->numChannels,
                                STEREO_LEFT_AUDIO_CHANNEL_IDX,
                                volumeMultiplier
                            );

                            audio::channel_audio_sample_container_t rightSample = getChannelSample(
                                chunkData,
                                formatChunk->sampleSizeBytes,
                                sampleIdx,
                                formatChunk->numChannels,
                                STEREO_RIGHT_AUDIO_CHANNEL_IDX,
                                volumeMultiplier
                            );

                            sampleData[sampleIdx] = audio::StereoAudioSampleContainer{leftSample, rightSample};
                        }
                    } else {
                        ASSERT(false);
                    }
                    return WaveData{
                        numSamples,
                        sampleData,
                    };
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
        return {};
    }
}
