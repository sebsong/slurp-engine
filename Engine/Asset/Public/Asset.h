#pragma once

#include "MemoryAllocator.h"
#include "Audio.h"
#include "Material.h"
#include "Mesh.h"
#include "Types.h"

#include <unordered_map>

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

        Sprite* loadSprite(
            const std::string& bitmapFileName
        );

        Sprite* loadSprite(
            const std::string& bitmapFileName,
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        );

        PlayingSound* loadSound(const std::string& waveFileName);

        ShaderSource* loadVertexShaderSource(const std::string& shaderSourceFileName);

        ShaderSource* loadFragmentShaderSource(const std::string& shaderSourceFileName);

        render::ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    private:
        std::hash<std::string> _stringHasher;

        template<typename key_t, typename value_t>
        using unordered_map_arena =
        std::unordered_map<
            key_t,
            value_t,
            std::hash<key_t>,
            std::equal_to<key_t>,
            memory::PermanentArenaAllocator<std::pair<const asset_id, Asset*>>
        >;

        unordered_map_arena<asset_id, Asset*> _assets;

        asset_id _getAssetId(const std::string& assetFilePath) const;

        Asset* _getAsset(asset_id assetId);

        void _registerAsset(asset_id assetId, Asset* asset);

        ShaderSource* _loadShaderSource(const std::string& shaderFilePath);
    };
}
