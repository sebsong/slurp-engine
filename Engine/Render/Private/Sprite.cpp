#include "Sprite.h"

#include <filesystem>

#include "WinEngine.h"

namespace render {
    void Sprite::draw(const GraphicsBuffer& buffer, const slurp::Vector2<float>& startPoint) const {
        const slurp::Vector2<int> roundedStartPoint = static_cast<slurp::Vector2<int>>(startPoint);
        const slurp::Vector2<float> endPoint = startPoint + bitmap.dimensions;
        const slurp::Vector2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vector2<int> clampedEndPoint = _getClamped(buffer, endPoint);

        for (int y = clampedStartPoint.y; y < clampedEndPoint.y; y++) {
            for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) {
                Pixel pixel = bitmap.map[(y - roundedStartPoint.y) * bitmap.dimensions.x + (x - roundedStartPoint.x)];
                _drawAtPoint(
                    buffer,
                    {x, y},
                    pixel
                );
            }
        }
    }

    static void loadBitmapColorPalette(
        slurp::byte* spriteFileBytes,
        slurp::byte* bitmapBytes,
        int width,
        int height,
        int colorPaletteSize,
        int bitsPerIndex,
        Pixel* outMap
    ) {
        assert(bitsPerIndex == 4);

        Pixel* colorPalette = reinterpret_cast<Pixel*>(spriteFileBytes + sizeof(BitmapHeader));
        for (int i = 0; i < colorPaletteSize; i++) {
            if (i == 0) {
                // NOTE: Assumes that the 0 index color is the transparent color
                colorPalette[i] = 0;
            } else {
                // NOTE: indexed colors don't support alpha channel (at least in aesprite)
                colorPalette[i] |= AlphaMask;
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
                slurp::byte colorIndex = bitmapBytes[x / 2 + y * rowSizeBytes];
                if (x % 2 == 0) { colorIndex = (colorIndex & FourBitMaskHigh) >> 4; } else {
                    colorIndex &= FourBitMaskLow;
                }
                Pixel color = colorPalette[colorIndex];
                outMap[x + ((height - 1) - y) * width] = color;
            }
        }
    }

    static void loadBitmapBitFields(
        slurp::byte* bitmapBytes,
        int width,
        int height,
        int bitsPerPixel,
        Pixel* outMap
    ) {
        assert(bitsPerPixel == 32);
        Pixel* pixels = reinterpret_cast<Pixel*>(bitmapBytes);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Pixel color = pixels[x + y * width];
                outMap[x + ((height - 1) - y) * width] = color;
            }
        }
    }

    // TODO: move this to windows layer?
    Sprite loadSprite(const std::string& spriteFileName) {
        const std::string filePath = SpritesDirectory + spriteFileName;
        std::ifstream file(filePath, std::ios::binary);
        assert(file.good());

        auto fileSize = std::filesystem::file_size(filePath);
        slurp::byte* fileBytes = new slurp::byte[fileSize];
        file.read(reinterpret_cast<std::istream::char_type*>(fileBytes), fileSize);
        BitmapHeader* header = reinterpret_cast<BitmapHeader*>(fileBytes);

        slurp::byte* bitmapBytes = fileBytes + header->fileHeader.bfOffBits;
        int width = static_cast<int>(header->infoHeader.biHeight);
        int height = static_cast<int>(header->infoHeader.biHeight);
        Pixel* map = new Pixel[width * height];
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
            assert(false);
        }

        return Sprite{
            Bitmap{
                {width, height},
                map
            }
        };
    }
}
