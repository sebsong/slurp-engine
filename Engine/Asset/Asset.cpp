#include "Asset.h"

#include "SlurpEngine.h"
#include "Bitmap.h"
#include "Debug.h"
#include "Types.h"
#include "Wave.h"

#include <filesystem>
#include <fstream>

namespace asset {
//TODO: need to package this with the build
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
#if PLATFORM_WINDOWS
    static const std::string AssetsDirectory = "../Assets/";
#elif PLATFORM_MAC
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";
    static const std::string SpritesDirectory = AssetsDirectory + "Sprites/";
    static const std::string SoundsDirectory = AssetsDirectory + "Sounds/";
    static const std::string ShadersDirectory = AssetsDirectory + "Shaders/";
    static const std::string VertexShadersDirectory = ShadersDirectory + "1_Vertex/";
    static const std::string FragmentShadersDirectory = ShadersDirectory + "2_Fragment/";

    AssetLoader::AssetLoader(): _stringHasher(std::hash<std::string>()),
                                _assets(types::unordered_map_arena<asset_id, Asset*>()) {}

    static FileReadResult readBytes(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);

        ASSERT(file.good());
        if (!file.good()) { return {}; }

        uint32_t fileSizeBytes = std::filesystem::file_size(filePath);
        types::byte* fileBytes = memory::AssetLoader->allocate(fileSizeBytes);
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

    ShaderSource* AssetLoader::_loadShaderSource(const std::string& shaderFilePath) {
        asset_id assetId = _getAssetId(shaderFilePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<ShaderSource*>(existingAsset);
        }

        ShaderSource* shaderSource = memory::AssetLoader->allocate<ShaderSource>();
        _registerAsset(assetId, shaderSource);

        std::string source = readTextFile(shaderFilePath);
        shaderSource->isLoaded = true;
        shaderSource->source = source;

        return shaderSource;
    }

    ShaderProgram* AssetLoader::loadShaderProgram(
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) {
        std::string vertexFilePath = VertexShadersDirectory + vertexShaderFileName;
        std::string fragmentFilePath = FragmentShadersDirectory + fragmentShaderFileName;
        asset_id assetId = _getAssetId(vertexFilePath + fragmentFilePath);
        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<ShaderProgram*>(existingAsset);
        }

        ShaderProgram* shaderProgram = memory::AssetLoader->allocate<ShaderProgram>();
        _registerAsset(assetId, shaderProgram);

        std::string vertexShaderSource = _loadShaderSource(vertexFilePath)->source;
        std::string fragmentShaderSource = _loadShaderSource(fragmentFilePath)->source;

        shaderProgram->isLoaded = true;
        shaderProgram->programId = slurp::Globals->RenderApi->createShaderProgram(
            vertexShaderSource.c_str(),
            fragmentShaderSource.c_str()
        );

        return shaderProgram;
    }

    Bitmap* AssetLoader::loadBitmap(const std::string& bitmapFileName) {
        std::string filePath = SpritesDirectory + bitmapFileName;
        asset_id assetId = _getAssetId(filePath);
        // if (Asset* existingAsset = _getAsset(assetId)) {
        //     return reinterpret_cast<Bitmap*>(existingAsset);
        // }

        Bitmap* bitmap = memory::AssetLoader->allocate<Bitmap>();
        _registerAsset(assetId, bitmap);

        // TODO: load this async?
        types::byte* fileBytes = readBytes(filePath).contents;
        if (!fileBytes) {
            return bitmap;
        }
        loadBitmapData(bitmap, fileBytes);

        return bitmap;
    }

    Sprite* AssetLoader::loadSprite(const std::string& bitmapFileName) {
        return loadSprite(
            bitmapFileName,
            DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME,
            DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME
        );
    }

    Sprite* AssetLoader::loadSprite(
        const std::string& bitmapFileName,
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName
    ) {
        std::string filePath = SpritesDirectory + bitmapFileName;
        asset_id assetId = _getAssetId(filePath + vertexShaderFileName + fragmentShaderFileName);

        // if (Asset* existingSprite = _getAsset(assetId)) {
        //     return reinterpret_cast<Sprite*>(existingSprite);
        // }

        Sprite* sprite = memory::Permanent->allocate<Sprite>();
        sprite->sourceFileName = bitmapFileName;
        _registerAsset(assetId, sprite);

        Bitmap* bitmap = asset::loadBitmap(bitmapFileName);

        render::object_id shaderProgramId = loadShaderProgram(vertexShaderFileName, fragmentShaderFileName)->programId;
        loadSpriteData(sprite, bitmap, shaderProgramId);

        return sprite;
    }

    SpriteAnimation* AssetLoader::loadSpriteAnimation(const std::string& bitmapFileName, uint8_t numFrames) {
        std::string filePath = SpritesDirectory + bitmapFileName;
        asset_id assetId = _getAssetId(filePath);

        // if (Asset* existingSpriteAnimation = _getAsset(assetId)) {
        //     return reinterpret_cast<SpriteAnimation*>(existingSprite);
        // }

        SpriteAnimation* animation = memory::Permanent->allocate<SpriteAnimation>();
        animation->sourceFileName = bitmapFileName;
        _registerAsset(assetId, animation);

        Bitmap* bitmap = asset::loadBitmap(bitmapFileName);

        loadSpriteAnimationData(animation, bitmap, numFrames);

        return animation;
    }

    // TODO: pre-process wave files into the engine sample size
    // TODO: stream the file in async
    Sound* AssetLoader::loadSound(const std::string& waveFileName) {
        std::string filePath = SoundsDirectory + waveFileName;
        asset_id assetId = _getAssetId(filePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<Sound*>(existingAsset);
        }

        Sound* sound = memory::Permanent->allocate<Sound>();
        _registerAsset(assetId, sound);

        auto loadFn = [sound, filePath]() {
            FileReadResult fileReadResult = readBytes(filePath);
            types::byte* fileBytes = fileReadResult.contents;
            ASSERT(fileBytes);
            if (!fileBytes) {
                return;
            }
            loadWaveData(sound, fileBytes, fileReadResult.sizeBytes);
        };
        job::queueJob(loadFn);

        return sound;
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
