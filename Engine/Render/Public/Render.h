#pragma once

#include "Vector.h"
#include "Geometry.h"

#include <cstdint>

#define COLOR_PALETTE_SIZE 9

namespace slurp {
    struct Entity;
}

namespace render {
    typedef uint8_t ColorPaletteIdx;
    typedef uint32_t Pixel;

    constexpr Pixel AlphaMask = 0xFF000000;
    constexpr uint8_t AlphaShift = 24;
    constexpr Pixel RedMask = 0x00FF0000;
    constexpr uint8_t RedShift = 16;
    constexpr Pixel GreenMask = 0x0000FF00;
    constexpr uint8_t GreenShift = 8;
    constexpr Pixel BlueMask = 0x000000FF;
    constexpr uint8_t BlueShift = 0;

    // TODO: move to game layer
    struct ColorPalette {
        Pixel colors[COLOR_PALETTE_SIZE];
    };

    // TODO: move to RenderInfo?
    struct RenderShape {
        geometry::Shape shape;
        Pixel color;

        void draw(const slurp::Vec2<float>& startPoint) const;
    };

    template<typename T>
    concept Renderable = requires(T renderable, const slurp::Vec2<float>& position) {
        { renderable.draw(position) } -> std::same_as<void>;
    };

    template<Renderable T>
    void drawRenderable(const T& renderable, const slurp::Vec2<float>& position) {
        renderable.draw(position);
    }

    inline Pixel withAlpha(Pixel color, float alpha);
}
