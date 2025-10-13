#pragma once
#include "Audio.h"
#include "Material.h"
#include "Mesh.h"
#include "Types.h"

namespace render {
    struct ColorPalette;
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

    // TODO: convert into async loaded asset::Asset
    struct Sprite : Asset {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;

        void draw(const slurp::Vec2<float>& startPoint) const;
    };

    struct PlayingSound : Asset {
        uint32_t numSamples;
        audio::StereoAudioSampleContainer* sampleData;
    };

    struct ShaderSource : Asset {
        std::string source;
    };

    class AssetLoader {
    public:
        AssetLoader();

        Bitmap* loadBitmap(const std::string& bitmapFileName);

        PlayingSound* loadSound(const std::string& waveFileName);

        ShaderSource* loadVertexShaderSource(const std::string& shaderSourceFileName);

        ShaderSource* loadFragmentShaderSource(const std::string& shaderSourceFileName);

        render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    private:
        std::hash<std::string> _stringHasher;
        std::unordered_map<asset_id, Asset*> _assets;

        asset_id _getAssetId(const std::string& assetFilePath) const;

        Asset* _getAsset(asset_id assetId);

        void _registerAsset(asset_id assetId, Asset* asset);
    };
}
