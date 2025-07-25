#pragma once
#include "Vector.h"
#include "Geometry.h"
#include <cstdint>

#include "Types.h"

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

#ifdef ASSETS_DIR
    static const std::string AssetsDirectory = ASSETS_DIR;
#else
    static const std::string AssetsDirectory = "../../../../Assets/";
#endif

    static const std::string PalettesDirectory = AssetsDirectory + "Palettes/";
    static const std::string SpritesDirectory = AssetsDirectory + "Sprites/";

    struct GraphicsBuffer {
        Pixel* const pixelMap; // memory byte order: XRGB
        int widthPixels;
        int heightPixels;
    };

    // TODO: move to game layer
    struct ColorPalette {
        Pixel colors[COLOR_PALETTE_SIZE];
    };

    // TODO: move to RenderInfo?
    struct RenderShape {
        geometry::Shape shape;
        Pixel color;

        void draw(const GraphicsBuffer& buffer, const slurp::Vector2<int>& startPoint) const;
    };

    template<typename T>
    concept Renderable = requires(T renderable, const GraphicsBuffer& buffer, const slurp::Vector2<int>& position) {
        { renderable.draw(buffer, position) } -> std::same_as<void>;
    };

    template<Renderable T>
    void drawRenderable(const GraphicsBuffer& buffer, const T& renderable, const slurp::Vector2<int>& position) {
        renderable.draw(buffer, position);
    }

    void drawRectBorder(
        const GraphicsBuffer& buffer,
        const slurp::Vector2<int>& startPoint,
        const slurp::Vector2<int>& endPoint,
        uint8_t borderThickness,
        uint32_t color
    );

    ColorPalette loadColorPalette(const std::string& paletteHexFileName);

    inline Pixel withAlpha(Pixel color, float alpha);
}
