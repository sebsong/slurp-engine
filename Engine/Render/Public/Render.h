#pragma once
#include "Vector.h"
#include "Geometry.h"
#include <cstdint>

#define COLOR_PALETTE_SIZE 8

namespace slurp {
    struct Entity;
}

namespace render {
    typedef uint8_t ColorPaletteIdx;
    typedef uint32_t Pixel;

    struct GraphicsBuffer {
        Pixel* const pixelMap; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
    };

    // TODO: move to game layer
    struct ColorPalette {
        Pixel colors[COLOR_PALETTE_SIZE];
    };

    struct RenderShape {
        geometry::Shape shape;
        slurp::Vector2<int> renderOffset;
        Pixel color;

        void draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) const;
    };

    template<typename T>
    concept Renderable = requires(T renderable) {
        { renderable.renderShape } -> std::same_as<RenderShape&>;
        { renderable.position } -> std::same_as<slurp::Vector2<int>&>;
    };

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        float r,
        float g,
        float b
    );

    void drawRect(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );

    void drawSquare(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& point,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );

    void drawLine(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        int size,
        ColorPaletteIdx colorPaletteIdx,
        const ColorPalette& colorPalette
    );

    template<Renderable T>
    void drawRenderable(const GraphicsBuffer& buffer, const T* renderable) {
        renderable->renderShape.draw(buffer, renderable->position);
    }

    void drawRectBorder(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        uint8_t borderThickness,
        uint32_t color
    );

    ColorPalette DEBUG_loadColorPalette(const std::string& paletteHexFileName);
}
