#include <SlurpEngine.hpp>

#include <iostream>

typedef unsigned char byte;

void main()
{
}

static void renderCoolGraphics(const GraphicsBuffer buffer, float xOffset, float yOffset)
{
    byte* bitmapBytes = static_cast<byte*>(buffer.memory);
    for (int y = 0; y < buffer.heightPixels; y++)
    {
        uint32_t* rowPixels = reinterpret_cast<uint32_t*>(bitmapBytes);
        for (int x = 0; x < buffer.widthPixels; x++)
        {
            uint8_t r = y + yOffset;
            uint8_t g = (x + xOffset) - (y + yOffset);
            uint8_t b = x + xOffset;

            uint32_t pixel = (r << 16) | (g << 8) | b;
            *rowPixels++ = pixel;
        }

        bitmapBytes += buffer.pitchBytes;
    }
}

void update(const GraphicsBuffer buffer, float xOffset, float yOffset)
{
    renderCoolGraphics(buffer, xOffset, yOffset);
}
