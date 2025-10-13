#include "Asset.h"

#include "Wave.h"
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

    AssetLoader::AssetLoader(): _stringHasher(std::hash<std::string>()),
                                _assets(std::unordered_map<asset_id, Asset*>()) {}

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

    static std::string readTextFile(const std::string& filePath) {
        std::ifstream file(filePath);
        ASSERT(file.good());

        std::ostringstream stream;
        stream << file.rdbuf();

        return stream.str();
    }

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
                if (x % 2 == 0) {
                    colorIndex = (colorIndex & FourBitMaskHigh) >> 4;
                } else {
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


    Bitmap AssetLoader::loadBitmapFile(const std::string& bitmapFileName) {
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

    // TODO: pre-process wave files into the engine sample size
    // TODO: stream the file in async
    Sound* AssetLoader::loadSound(const std::string& waveFileName) {
        asset_id assetId = _getAssetId(waveFileName);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<Sound*>(existingAsset);
        }

        Sound* sound = new Sound();
        _registerAsset(assetId, sound);

        // TODO: do this async
        FileReadResult fileReadResult = readBytes(SoundsDirectory, waveFileName);
        types::byte* fileBytes = fileReadResult.contents; // TODO: free the memory
        ASSERT(fileBytes);
        if (!fileBytes) {
            return sound;
        }

        loadWaveData(*sound, fileBytes, fileReadResult.sizeBytes);

        delete[] fileBytes;
        return sound;
    }

    ShaderSource* AssetLoader::loadVertexShaderSource(const std::string& shaderSourceFileName) {
        std::string filePath = VertexShadersDirectory + shaderSourceFileName;
        asset_id assetId = _getAssetId(filePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<ShaderSource*>(existingAsset);
        }

        ShaderSource* shaderSource = new ShaderSource();
        _registerAsset(assetId, shaderSource);

        std::string source = readTextFile(filePath);
        shaderSource->source = source;

        return shaderSource;
    }

    ShaderSource* AssetLoader::loadFragmentShaderSource(const std::string& shaderSourceFileName) {
        std::string filePath = FragmentShadersDirectory + shaderSourceFileName;
        asset_id assetId = _getAssetId(filePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<ShaderSource*>(existingAsset);
        }

        ShaderSource* shaderSource = new ShaderSource();
        _registerAsset(assetId, shaderSource);

        std::string source = readTextFile(filePath);
        shaderSource->source = source;

        return shaderSource;
    }

    render::ColorPalette AssetLoader::loadColorPalette(const std::string& paletteHexFileName) {
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

    asset_id AssetLoader::_getAssetId(const std::string& assetFilePath) const {
        return _stringHasher(assetFilePath);
    }

    Asset* AssetLoader::_getAsset(asset_id assetId) {
        return _assets[assetId];
    }

    void AssetLoader::_registerAsset(asset_id assetId, Asset* asset) {
        asset->id = assetId;
        _assets[assetId] = asset;
    }
}
