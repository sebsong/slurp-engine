#include "Asset.h"

#include "SlurpEngine.h"
#include "Bitmap.h"
#include "Wave.h"
#include "Debug.h"
#include "Types.h"

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

    AssetLoader::AssetLoader(): _stringHasher(std::hash<std::string>()),
                                _assets(types::unordered_map_arena<asset_id, Asset*>()),
                                _assetFilePaths(types::unordered_map_arena<asset_id, std::string>()) {}

    static FileReadResult readBytes(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);

        ASSERT(file.good());
        if (!file.good()) { return {}; }

        uint32_t fileSizeBytes = std::filesystem::file_size(filePath);
        types::byte* fileBytes = memory::GlobalGameMemory.assetLoader.allocate(fileSizeBytes);
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

    Bitmap* AssetLoader::loadBitmap(const std::string& bitmapFileName) {
        std::string filePath = SpritesDirectory + bitmapFileName;
        asset_id assetId = _getAssetId(filePath);
        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<Bitmap*>(existingAsset);
        }

        Bitmap* bitmap = memory::GlobalGameMemory.assetLoader.allocate<Bitmap>();
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

        if (Asset* existingSprite = _getAsset(assetId)) {
            return reinterpret_cast<Sprite*>(existingSprite);
        }

        Sprite* sprite = memory::GlobalGameMemory.permanent.allocate<Sprite>();
        _registerAsset(assetId, sprite);

        Bitmap* bitmap = slurp::GlobalAssetLoader->loadBitmap(bitmapFileName);
        std::string vertexShaderSource =
                slurp::GlobalAssetLoader->loadVertexShaderSource(vertexShaderFileName)->source;
        std::string fragmentShaderSource =
                slurp::GlobalAssetLoader->loadFragmentShaderSource(fragmentShaderFileName)->source;

        loadSpriteData(sprite, bitmap, vertexShaderSource, fragmentShaderSource);

        return sprite;
    }

    // TODO: pre-process wave files into the engine sample size
    // TODO: stream the file in async
    Sound* AssetLoader::loadSound(const std::string& waveFileName) {
        std::string filePath = SoundsDirectory + waveFileName;
        asset_id assetId = _getAssetId(filePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<Sound*>(existingAsset);
        }

        Sound* sound = memory::GlobalGameMemory.permanent.allocate<Sound>();
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
        slurp::GlobalJobRunner->queueJob(loadFn);

        return sound;
    }

    ShaderSource* AssetLoader::_loadShaderSource(const std::string& shaderFilePath) {
        asset_id assetId = _getAssetId(shaderFilePath);

        if (Asset* existingAsset = _getAsset(assetId)) {
            return reinterpret_cast<ShaderSource*>(existingAsset);
        }

        ShaderSource* shaderSource = memory::GlobalGameMemory.assetLoader.allocate<ShaderSource>();
        _registerAsset(assetId, shaderSource);

        std::string source = readTextFile(shaderFilePath);
        shaderSource->isLoaded = true;
        shaderSource->source = source;

        return shaderSource;
    }

    ShaderSource* AssetLoader::loadVertexShaderSource(const std::string& shaderSourceFileName) {
        std::string filePath = VertexShadersDirectory + shaderSourceFileName;
        return _loadShaderSource(filePath);
    }

    ShaderSource* AssetLoader::loadFragmentShaderSource(const std::string& shaderSourceFileName) {
        std::string filePath = FragmentShadersDirectory + shaderSourceFileName;
        return _loadShaderSource(filePath);
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

    void AssetLoader::_registerAssetFilePath(asset_id assetId, const std::string& filePath) {
        _assetFilePaths[assetId] = filePath;
    }

    std::string AssetLoader::getAssetFilePath(asset_id assetId) const {
        auto it = _assetFilePaths.find(assetId);
        if (it != _assetFilePaths.end()) {
            return it->second;
        }
        return "";
    }

    // Hot reload methods
    void AssetLoader::reloadAsset(const std::string& assetFilePath) {
        asset_id assetId = _getAssetId(assetFilePath);
        Asset* asset = _getAsset(assetId);

        if (!asset || !asset->isLoaded) {
            return; // Asset not loaded yet
        }

        // Determine asset type and reload accordingly
        if (assetFilePath.find(".bmp") != std::string::npos) {
            Bitmap* bitmap = reinterpret_cast<Bitmap*>(asset);
            std::string fileName = assetFilePath.substr(assetFilePath.find_last_of("/") + 1);
            reloadBitmap(bitmap, fileName);
        } else if (assetFilePath.find(".wav") != std::string::npos) {
            Sound* sound = reinterpret_cast<Sound*>(asset);
            std::string fileName = assetFilePath.substr(assetFilePath.find_last_of("/") + 1);
            reloadSound(sound, fileName);
        } else if (assetFilePath.find(".glsl") != std::string::npos) {
            ShaderSource* shader = reinterpret_cast<ShaderSource*>(asset);
            std::string fileName = assetFilePath.substr(assetFilePath.find_last_of("/") + 1);
            reloadShader(shader, fileName);
        }
    }

    void AssetLoader::reloadBitmap(Bitmap* bitmap, const std::string& bitmapFileName) {
        std::string filePath = SpritesDirectory + bitmapFileName;
        types::byte* fileBytes = readBytes(filePath).contents;
        if (!fileBytes) {
            return;
        }
        loadBitmapData(bitmap, fileBytes);
    }

    void AssetLoader::reloadSprite(Sprite* sprite, const std::string& bitmapFileName) {
        // Reload the bitmap and update the texture on GPU
        if (slurp::GlobalRenderApi) {
            Bitmap* bitmap = loadBitmap(bitmapFileName);
            if (bitmap && bitmap->isLoaded) {
                slurp::GlobalRenderApi->updateTexture(sprite->material.textureId, bitmap);
            }
        }
    }

    void AssetLoader::reloadSound(Sound* sound, const std::string& waveFileName) {
        std::string filePath = SoundsDirectory + waveFileName;
        types::byte* fileBytes = readBytes(filePath).contents;
        if (!fileBytes) {
            return;
        }

        // Free old sound data if needed
        // Note: We're reusing the same pointer, so the Sound* remains valid
        loadWaveData(sound, fileBytes, readBytes(filePath).sizeBytes);
    }

    void AssetLoader::reloadShader(ShaderSource* shader, const std::string& shaderFileName) {
        std::string filePath;
        if (shaderFileName.find("Vertex") != std::string::npos || shaderFileName.find("1_") != std::string::npos) {
            filePath = VertexShadersDirectory + shaderFileName;
        } else {
            filePath = FragmentShadersDirectory + shaderFileName;
        }

        shader->source = readTextFile(filePath);
        shader->isLoaded = true;

        // Note: Shader program recompilation will need to be triggered separately
        // This is handled in the OpenGL layer
    }
}
