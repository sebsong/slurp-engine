#pragma once

#include "MemoryAllocator.h"
#include "Audio.h"
#include "Material.h"
#include "Mesh.h"
#include "CollectionTypes.h"

namespace render {
    struct ColorPalette;
}

namespace asset {
    struct Bitmap;

    typedef uint32_t asset_id;

    struct FileReadResult {
        uint32_t sizeBytes;
        types::byte* contents;
    };

    struct Asset {
        uint32_t id;
        bool isLoaded;
        // TODO: ref count
    };

    struct Sprite : Asset {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;

        void draw(const slurp::Vec2<float>& startPoint) const;
    };

    struct Sound : Asset {
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

        Sprite* loadSprite(
            const std::string& bitmapFileName
        );

        Sprite* loadSprite(
            const std::string& bitmapFileName,
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        );

        Sound* loadSound(const std::string& waveFileName);

        ShaderSource* loadVertexShaderSource(const std::string& shaderSourceFileName);

        ShaderSource* loadFragmentShaderSource(const std::string& shaderSourceFileName);

        render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

        // Hot reload support
        void reloadAsset(const std::string& assetFilePath);
        void reloadBitmap(Bitmap* bitmap, const std::string& bitmapFileName);
        void reloadSprite(Sprite* sprite, const std::string& bitmapFileName);
        void reloadSound(Sound* sound, const std::string& waveFileName);
        void reloadShader(ShaderSource* shader, const std::string& shaderFileName);

        // Get asset file path for hot reload tracking
        std::string getAssetFilePath(asset_id assetId) const;

    private:
        std::hash<std::string> _stringHasher;
        types::unordered_map_arena<asset_id, Asset*> _assets;
        types::unordered_map_arena<asset_id, std::string> _assetFilePaths; // Track file paths for hot reload

        asset_id _getAssetId(const std::string& assetFilePath) const;

        Asset* _getAsset(asset_id assetId);

        void _registerAsset(asset_id assetId, Asset* asset);

        void _registerAssetFilePath(asset_id assetId, const std::string& filePath);

        ShaderSource* _loadShaderSource(const std::string& shaderFilePath);
    };
}
