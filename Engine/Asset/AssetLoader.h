#pragma once
#include <string>

#include "Asset.h"
#include "CollectionTypes.h"

namespace asset {
    struct Sprite;
    struct SpriteAnimation;

    class AssetLoader {
    public:
        AssetLoader();

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

        Sound* loadSound(const std::string& waveFileName);

    private:
        std::hash<std::string> _stringHasher;
        types::unordered_map_arena<asset_id, Asset*> _assets;

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

    inline Sound* loadSound(const std::string& waveFileName) {
        return slurp::Globals->AssetLoader->loadSound(waveFileName);
    }
}
