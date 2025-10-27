#pragma once

#include "MemoryAllocator.h"
#include "Audio.h"
#include "Material.h"
#include "Mesh.h"
#include "CollectionTypes.h"

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

        void draw(const slurp::Vec2<float>& startPoint, int zOrder) const;
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

    private:
        std::hash<std::string> _stringHasher;
        types::unordered_map_arena<asset_id, Asset*> _assets;

        asset_id _getAssetId(const std::string& assetFilePath) const;

        Asset* _getAsset(asset_id assetId);

        void _registerAsset(asset_id assetId, Asset* asset);

        ShaderSource* _loadShaderSource(const std::string& shaderFilePath);
    };

    static Bitmap* loadBitmap(const std::string& bitmapFileName);

    static Sprite* loadSprite(
        const std::string& bitmapFileName
    );

    static Sprite* loadSprite(
        const std::string& bitmapFileName,
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    );

    static Sound* loadSound(const std::string& waveFileName);

    static ShaderSource* loadVertexShaderSource(const std::string& shaderSourceFileName);

    static ShaderSource* loadFragmentShaderSource(const std::string& shaderSourceFileName);
}
