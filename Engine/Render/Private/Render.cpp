#include "Render.h"

#include "Entity.h"
#include "Math.h"
#include "Debug.h"

#include <filesystem>
#include <fstream>
#include <string>

#include "WinEngine.h"

namespace render {
#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";
    static const std::string SpritesDirectory = AssetsDirectory + "Sprites/";

    constexpr Pixel AlphaMask = 0xFF000000;
    constexpr uint8_t FourBitMaskLow = 0b00001111;
    constexpr uint8_t FourBitMaskHigh = 0b11110000;

    static void _drawAtPoint(const GraphicsBuffer& buffer, const slurp::Vector2<int>& point, Pixel color) {
        uint8_t alpha = color >> 24;
        if (alpha == 0) { return; }
        *(buffer.pixelMap + point.x + (point.y * buffer.widthPixels)) = color;
    }

    static slurp::Vector2<int> _getClamped(const GraphicsBuffer& buffer, const slurp::Vector2<int>& point) {
        return {
            std::min(std::max(point.x, 0), buffer.widthPixels),
            std::min(std::max(point.y, 0), buffer.heightPixels)
        };
    }

    static void _drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        Pixel color
    ) {
        const slurp::Vector2<int> clampedStartPoint = _getClamped(buffer, startPoint);
        const slurp::Vector2<int> clampedEndPoint = _getClamped(buffer, endPoint);
        for (int y = clampedStartPoint.y; y < clampedEndPoint.y; y++) {
            for (int x = clampedStartPoint.x; x < clampedEndPoint.x; x++) { _drawAtPoint(buffer, {x, y}, color); }
        }
    }

    static void _drawSquare(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& point,
        int size,
        Pixel color
    ) {
        _drawRect(
            buffer,
            point,
            {point.x + size, point.y + size},
            color
        );
    }

    static void _drawLine(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        int size,
        Pixel color
    ) {
        const float radius = static_cast<float>(size) / 2;
        const slurp::Vector2<int> sizeOffset = slurp::Vector2<int>::Unit * -math::getHypotenuse(radius, radius / 2);
        const slurp::Vector2<int> offsetStartPoint = startPoint + sizeOffset;
        const slurp::Vector2<int> offsetEndPoint = endPoint + sizeOffset;

        const slurp::Vector2<int> startToEnd = offsetEndPoint - offsetStartPoint;
        const slurp::Vector2<float> direction = static_cast<slurp::Vector2<float>>(startToEnd).normalize();

        slurp::Vector2<float> currentPoint = offsetStartPoint;
        float distance = startToEnd.magnitude();
        while (distance > 0) {
            _drawSquare(
                buffer,
                currentPoint,
                size,
                color
            );
            currentPoint += direction;
            distance--;
        }
    }

    void drawRectBorder(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        const uint8_t borderThickness,
        const Pixel color
    ) {
        _drawLine(
            buffer,
            startPoint,
            {endPoint.x, startPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            {endPoint.x, startPoint.y},
            endPoint,
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            endPoint,
            {startPoint.x, endPoint.y},
            borderThickness,
            color
        );
        _drawLine(
            buffer,
            {startPoint.x, endPoint.y},
            startPoint,
            borderThickness,
            color
        );
    }

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

    void RenderShape::draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const {
        const slurp::Vector2<int> endPoint = startPoint + shape.dimensions;
        switch (shape.type) {
            case geometry::Rect: {
                _drawRect(buffer, startPoint, endPoint, color);
                break;
            }
            default: { assert(false); }
        }
    }

    ColorPalette loadColorPalette(const std::string& paletteHexFileName) {
        ColorPalette palette = {};

        const std::string filePath = PalettesDirectory + paletteHexFileName;
        std::ifstream file(filePath);
        assert(file.good());

        uint8_t colorPaletteIdx = 0;
        std::string line;
        while (std::getline(file, line) && colorPaletteIdx < COLOR_PALETTE_SIZE) {
            Pixel color = std::stoi(line, nullptr, 16) | AlphaMask;
            palette.colors[colorPaletteIdx] = color;
            colorPaletteIdx++;
        }

        return palette;
    }

    static Pixel toSlurpPixel(const Pixel& bmpPixel) { return (bmpPixel >> 8) | (bmpPixel << 24); }
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
