#include "Asset.h"

#include <filesystem>
#include <fstream>

#include "Debug.h"
#include "Types.h"
#include "WinEngine.h"

namespace asset {
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";
    static const std::string SpritesDirectory = AssetsDirectory + "Sprites/";
    static const std::string SoundsDirectory = AssetsDirectory + "Sounds/";

    static constexpr uint8_t FourBitMaskLow = 0b00001111;
    static constexpr uint8_t FourBitMaskHigh = 0b11110000;

    static types::byte* readBytes(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);

        ASSERT(file.good());
        if (!file.good()) { return nullptr; }

        auto fileSize = std::filesystem::file_size(filePath);
        types::byte* fileBytes = new types::byte[fileSize]; // TODO: need to free this memory
        file.read(reinterpret_cast<std::istream::char_type*>(fileBytes), fileSize);

        return fileBytes;
    }

    static void loadBitmapColorPalette(
        types::byte* spriteFileBytes,
        types::byte* bitmapBytes,
        int width,
        int height,
        int colorPaletteSize,
        int bitsPerIndex,
        render::Pixel* outMap
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
                outMap[x + ((height - 1) - y) * width] = color;
            }
        }
    }

    static void loadBitmapBitFields(
        types::byte* bitmapBytes,
        int width,
        int height,
        int bitsPerPixel,
        render::Pixel* outMap
    ) {
        ASSERT(bitsPerPixel == 32);
        render::Pixel* pixels = reinterpret_cast<render::Pixel*>(bitmapBytes);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                render::Pixel color = pixels[x + y * width];
                outMap[x + ((height - 1) - y) * width] = color;
            }
        }
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

    // TODO: move this to windows layer?
    Bitmap loadBitmapFile(const std::string& bitmapFileName) {
        const std::string filePath = SpritesDirectory + bitmapFileName;
        types::byte* fileBytes = readBytes(filePath);
        if (!fileBytes) {
            return Bitmap{};
        }

        BitmapHeader* header = reinterpret_cast<BitmapHeader*>(fileBytes);

        types::byte* bitmapBytes = fileBytes + header->fileHeader.bfOffBits;
        int width = static_cast<int>(header->infoHeader.biHeight);
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

    static int64_t upsizeInt(int64_t num, int8_t numBytes, int8_t targetNumBytes) {
        ASSERT(numBytes <= targetNumBytes);
        ASSERT(targetNumBytes <= 8);

        if (numBytes == targetNumBytes) {
            return num;
        }

        int64_t result = num;
        uint8_t numBits = numBytes * BITS_PER_BYTE;
        uint8_t targetNumBits = targetNumBytes * BITS_PER_BYTE;
        int64_t signBitMask = 1 << (numBits - 1);
        int64_t targetTwosComplementMask = 1 << (targetNumBits - 1) >> (targetNumBits - numBits);
        if (num & signBitMask) {
            result |= targetTwosComplementMask;
        }
        return result;
    }

    static audio::audio_sample_t getChannelSample(
        const WaveChunks* chunks,
        uint32_t sampleIdx,
        uint64_t volumeMultiplier
    ) {
        // TODO: handle multiple channel source
        audio::audio_sample_t sample = 0;
        uint32_t byteOffset = sampleIdx * chunks->formatChunk.sampleSizeBytes;
        std::copy_n(
            chunks->data + byteOffset,
            chunks->formatChunk.sampleSizeBytes,
            reinterpret_cast<types::byte*>(&sample)
        );
        sample = upsizeInt(
            sample,
            chunks->formatChunk.sampleSizeBytes,
            sizeof(audio::audio_sample_t) / NUM_AUDIO_CHANNELS
        );
        return sample * volumeMultiplier;
    }

    WaveData loadWaveFile(const std::string& waveFileName) {
        const std::string filePath = SoundsDirectory + waveFileName;
        types::byte* fileBytes = readBytes(filePath);
        if (!fileBytes) {
            return WaveData{};
        }
        WaveChunks* chunks = reinterpret_cast<WaveChunks*>(fileBytes);

        // NOTE: coupled with platform audio buffer settings
#if DEBUG
        FormatChunk formatChunk = chunks->formatChunk;
        ASSERT(formatChunk.formatTag == WAVE_FORMAT_PCM);
        ASSERT(formatChunk.numChannels <= 2);
        ASSERT(formatChunk.sampleSizeBytes <= sizeof(audio::audio_sample_t));
        ASSERT(
            (formatChunk.sampleSizeBytes / formatChunk.numChannels) <=
            (sizeof(audio::audio_sample_t) / NUM_AUDIO_CHANNELS)
        );
        ASSERT(NUM_AUDIO_CHANNELS == 2);
        ASSERT(IS_TWOS_COMPLEMENT);
#endif

        DataChunkHeader dataChunkHeader = chunks->dataChunkHeader;
        uint32_t numSamples = dataChunkHeader.chunkSizeBytes / formatChunk.sampleSizeBytes;
        audio::audio_sample_t* sampleData = new audio::audio_sample_t[numSamples];

        uint8_t destNumBits = sizeof(audio::audio_sample_t) * BITS_PER_BYTE;
        uint64_t volumeMultiplier = types::maxSignedValue(sizeof(audio::audio_sample_t)) /
                                    types::maxSignedValue(formatChunk.sampleSizeBytes);
        if (formatChunk.numChannels == 1) {
            for (uint32_t sampleIdx = 0; sampleIdx < numSamples; sampleIdx++) {
                audio::audio_sample_t sample = getChannelSample(
                    chunks,
                    sampleIdx,
                    volumeMultiplier
                );

                // TODO: debug why it sounds crunchy now
                sampleData[sampleIdx] = (sample << (destNumBits / 2)) | sample;
            }
        } else if (formatChunk.numChannels == 2) {
            // TODO: handle stereo input file
        } else {
            ASSERT(false);
        }
        return WaveData{
            numSamples,
            sampleData,
        };
    }
}
