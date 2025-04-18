#pragma once

struct GraphicsBuffer
{
    void* memory; // memory byte order: XRGB
    int widthPixels;
    int heightPixels;
    int pitchBytes;
};

void main();

void update(GraphicsBuffer buffer, float xOffset, float yOffset);
