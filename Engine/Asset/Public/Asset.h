#pragma once
#include "Audio.h"
#include "Types.h"

namespace render {
    struct ColorPalette;
    struct Sprite;
}

namespace asset {
    struct Bitmap;

    typedef uint32_t asset_id;

    // TODO: we should stream assets in async
    struct FileReadResult {
        uint32_t sizeBytes;
        types::byte* contents;
    };

    struct Asset {
        uint32_t id;
        bool isLoaded;
        // TODO: ref count
    };

    struct Sound: Asset {
        uint32_t numSamples;
        audio::StereoAudioSampleContainer* sampleData;
    };

    class AssetLoader {
    public:
        AssetLoader();

        Bitmap loadBitmapFile(const std::string& bitmapFileName);

        Sound* loadWaveFile(const std::string& waveFileName);

        std::string loadVertexShaderSource(const std::string& shaderSourceFileName);

        std::string loadFragmentShaderSource(const std::string& shaderSourceFileName);

        render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    private:
        asset_id _nextAssetId;
        std::unordered_map<asset_id, Asset*> _assets;

        asset_id _registerAsset(Asset* asset);
    };
}
