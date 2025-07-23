#include "Sprite.h"

#include <filesystem>

namespace render {
    void Sprite::draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const {
        const slurp::Vector2<int> endPoint = startPoint + bitmap.dimensions;
        const slurp::Vector2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vector2<int> clampedEndPoint = _getClamped(buffer, endPoint);
        const int clampedWidth = clampedEndPoint.x - clampedStartPoint.x;
        const int clampedHeight = clampedEndPoint.y - clampedStartPoint.y;

        for (int y = 0; y < clampedHeight; y++) {
            for (int x = 0; x < clampedWidth; x++) {
                Pixel pixel = bitmap.map[x + y * bitmap.dimensions.x];
                _drawAtPoint(
                    buffer,
                    clampedStartPoint + slurp::Vector2{x, y},
                    pixel
                );
            }
        }
    }

    // TODO: move this to windows layer?
    Sprite loadSprite(const std::string& spriteFileName) {
        const std::string filePath = SpritesDirectory + spriteFileName;
        std::ifstream file(filePath, std::ios::binary);
        assert(file.good());

        auto fileSize = std::filesystem::file_size(filePath);
        slurp::byte* bytes = new slurp::byte[fileSize];
        file.read(reinterpret_cast<std::istream::char_type*>(bytes), fileSize);
        BitmapHeader* header = reinterpret_cast<BitmapHeader*>(bytes);

        if (header->infoHeader.biCompression == BI_RGB && header->infoHeader.biBitCount <= 8) {
            // TODO: handle 8 bit count
            assert(header->infoHeader.biBitCount == 4);

            Pixel* colorPalette = reinterpret_cast<Pixel*>(bytes + sizeof(BitmapHeader));
            for (int i = 0; i < static_cast<int>(header->infoHeader.biClrUsed); i++) {
                if (i == 0) {
                    // NOTE: Assumes that the 0 index color is the transparent color
                    colorPalette[i] = 0;
                }
                else {
                    // NOTE: indexed colors don't support alpha channel (at least in aesprite)
                    colorPalette[i] |= AlphaMask;
                }
            }
            slurp::byte* colorIndicesBytes = bytes + header->fileHeader.bfOffBits;

            Pixel* map = new Pixel[header->infoHeader.biWidth * header->infoHeader.biHeight];
            int j = 0;
            int width = static_cast<int>(header->infoHeader.biHeight);
            int height = static_cast<int>(header->infoHeader.biHeight);
            int rowSizeBits = static_cast<int>(header->infoHeader.biWidth) * header->infoHeader.biBitCount;
            int rowSizeBytes = rowSizeBits / 8 + (rowSizeBits % 8 != 0);
            rowSizeBytes += 4 - (rowSizeBytes % 4); // NOTE: bitmaps are padded to 4 byte words
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    // TODO: avoid an extra read by re-using this for low + high bitmasking
                    slurp::byte colorIndex = colorIndicesBytes[x / 2 + y * rowSizeBytes];
                    if (x % 2 == 0) { colorIndex = (colorIndex & FourBitMaskHigh) >> 4; }
                    else {
                        colorIndex &= FourBitMaskLow;
                        j++;
                    }
                    Pixel color = colorPalette[colorIndex];
                    map[x + ((height - 1) - y) * width] = color;
                }
            }

            return Sprite{
                {
                    {
                        width,
                        height,
                    },
                    map
                }
            };
        }

        // TODO: handle non color table compressed bitmaps
        assert(false);
        return {};
    }
}
