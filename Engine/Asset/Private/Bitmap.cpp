#include "Bitmap.h"

#include <windef.h>
#include <wingdi.h>

namespace asset {
    static constexpr uint8_t FourBitMaskLow = 0b00001111;
    static constexpr uint8_t FourBitMaskHigh = 0b11110000;

    static void writePixel(
        int x,
        int y,
        int width,
        int height,
        render::Pixel color,
        render::Pixel* outMap,
        bool invertVertical
    ) {
        int yCoord = invertVertical ? (height - 1 - y) : y;
        outMap[x + yCoord * width] = color;
    }

    static void loadBitmapColorPalette(
        types::byte* bitmapFileBytes,
        const types::byte* bitmapBytes,
        int width,
        int height,
        int colorPaletteSize,
        int bitsPerIndex,
        render::Pixel* outMap,
        bool invertVertical = false
    ) {
        ASSERT(bitsPerIndex == 4);

        render::Pixel* colorPalette = reinterpret_cast<render::Pixel*>(bitmapFileBytes + sizeof(BitmapHeader));
        for (int i = 0; i < colorPaletteSize; i++) {
            if (i == 0) {
                // NOTE: Assumes that the 0 index color is the transparent color
                colorPalette[i] = 0;
            } else {
                // NOTE: indexed colors don't support alpha channel (at least in aesprite)
                colorPalette[i] |= render::AlphaMask;
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
                types::byte colorIndex = bitmapBytes[x / 2 + y * rowSizeBytes];
                if (x % 2 == 0) {
                    colorIndex = (colorIndex & FourBitMaskHigh) >> 4;
                } else {
                    colorIndex &= FourBitMaskLow;
                }
                render::Pixel color = colorPalette[colorIndex];
                writePixel(
                    x,
                    y,
                    width,
                    height,
                    color,
                    outMap,
                    invertVertical
                );
            }
        }
    }

    static void loadBitmapBitFields(
        const types::byte* bitmapBytes,
        int width,
        int height,
        int bitsPerPixel,
        render::Pixel* outMap,
        bool invertVertical = false
    ) {
        ASSERT(bitsPerPixel == 32);
        const render::Pixel* pixels = reinterpret_cast<const render::Pixel*>(bitmapBytes);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                render::Pixel color = pixels[x + y * width];
                writePixel(
                    x,
                    y,
                    width,
                    height,
                    color,
                    outMap,
                    invertVertical
                );
            }
        }
    }

    void loadBitmapData(Bitmap* bitmap, types::byte* bitmapFileBytes) {
        BitmapHeader* header = reinterpret_cast<BitmapHeader*>(bitmapFileBytes);

        const types::byte* bitmapBytes = bitmapFileBytes + header->fileHeader.bfOffBits;
        int width = static_cast<int>(header->infoHeader.biWidth);
        int height = static_cast<int>(header->infoHeader.biHeight);
        render::Pixel* map = memory::AssetLoader->allocate<render::Pixel>(width * height);

        if (header->infoHeader.biCompression == BI_RGB && header->infoHeader.biBitCount <= 8) {
            loadBitmapColorPalette(
                bitmapFileBytes,
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
            ASSERT(false);
        }

        bitmap->dimensions = {width, height};
        bitmap->map = map;
        bitmap->isLoaded = true;
    }

    BitmapSheet sliceBitmap(const Bitmap* bitmap, uint8_t numSlices) {
        Bitmap* bitmaps = memory::AssetLoader->allocate<Bitmap>(numSlices);
        slurp::Vec2 sliceDimensions = {bitmap->dimensions.width / numSlices, bitmap->dimensions.height};
        for (uint8_t sliceIdx = 0; sliceIdx < numSlices; sliceIdx++) {
            Bitmap& bitmapSlice = bitmaps[sliceIdx];
            bitmapSlice.dimensions = sliceDimensions;
            bitmapSlice.map = memory::AssetLoader->allocate<render::Pixel>(
                sliceDimensions.width * sliceDimensions.height
            );
            int xOffset = sliceDimensions.width * sliceIdx;
            for (int y = 0; y < sliceDimensions.height; y++) {
                for (int x = 0; x < sliceDimensions.width; x++) {
                    bitmapSlice.map[x + (y * sliceDimensions.width)] =
                            bitmap->map[x + xOffset + (y * bitmap->dimensions.width)];
                }
            }
        }

        return BitmapSheet{numSlices, bitmaps};
    }
}
