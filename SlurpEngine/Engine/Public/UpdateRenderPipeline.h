#pragma once
#include <queue>

namespace render {
    struct GraphicsBuffer;
    struct ColorPalette;
}

namespace slurp {
    struct Entity;

    class UpdateRenderPipeline {
    public:
        explicit UpdateRenderPipeline(const render::ColorPalette& colorPalette);

        Entity& initAndRegister(
            Entity& outEntity,
            std::string&& name,
            const Vector2<int>& position,
            const geometry::Shape& renderShape,
            render::ColorPaletteIdx color,
            bool centerPosition
        );

        void process(const render::GraphicsBuffer& buffer, float dt);

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
        const render::ColorPalette _colorPalette;
    };
}
