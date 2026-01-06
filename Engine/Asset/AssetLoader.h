#pragma once
#include <string>

#include "Asset.h"
#include "CollectionTypes.h"

#define COLOR_PALETTE_SIZE 9

struct MIX_Mixer;

namespace asset {
    struct Sprite;
    struct SpriteAnimation;

    typedef std::array<slurp::Vec4<float>, COLOR_PALETTE_SIZE> color_palette;

    class AssetLoader {
    public:
        explicit AssetLoader(MIX_Mixer* audioMixer);

        ShaderProgram* loadShaderProgram(
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        );

        Bitmap* loadBitmap(const std::string& bitmapFileName);

        Sprite* loadSprite(const std::string& bitmapFileName);

        Sprite* loadSprite(
            const std::string& bitmapFileName,
            const std::string& vertexShaderFileName,
            const std::string& fragmentShaderFileName
        );

        SpriteAnimation* loadSpriteAnimation(const std::string& bitmapFileName, uint8_t numFrames);

        color_palette* loadColorPalette(const std::string& hexFileName);

        Sound* loadSound(
            const std::string& waveFileName,
            audio::sound_group_id groupId
        );

    private:
        types::unordered_map_arena<asset_id, Asset*> _assets;
        std::hash<std::string> _stringHasher;
        MIX_Mixer* _audioMixer;

        asset_id _getAssetId(const std::string& assetFilePath) const;

        Asset* _getAsset(asset_id assetId);

        void _registerAsset(asset_id assetId, Asset* asset);

        ShaderSource* _loadShaderSource(const std::string& shaderFilePath);
    };

    inline ShaderProgram* loadShaderProgram(
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) {
        return slurp::Globals->AssetLoader->loadShaderProgram(vertexShaderFileName, fragmentShaderFileName);
    }

    inline Bitmap* loadBitmap(const std::string& bitmapFileName) {
        return slurp::Globals->AssetLoader->loadBitmap(bitmapFileName);
    }

    inline Sprite* loadSprite(const std::string& bitmapFileName) {
        return slurp::Globals->AssetLoader->loadSprite(bitmapFileName);
    }

    inline Sprite* loadSprite(
        const std::string& bitmapFileName,
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) {
        return slurp::Globals->AssetLoader->loadSprite(
            bitmapFileName,
            vertexShaderFileName,
            fragmentShaderFileName
        );
    }

    inline SpriteAnimation* loadSpriteAnimation(const std::string& bitmapFileName, uint8_t numFrames) {
        return slurp::Globals->AssetLoader->loadSpriteAnimation(bitmapFileName, numFrames);
    }

    inline color_palette* loadColorPalette(const std::string& hexFileName) {
        return slurp::Globals->AssetLoader->loadColorPalette(hexFileName);
    }

    inline Sound* loadSound(const std::string& waveFileName, audio::sound_group_id groupId = AUDIO_SOUND_GROUP_OTHER) {
        return slurp::Globals->AssetLoader->loadSound(waveFileName, groupId);
    }
}
