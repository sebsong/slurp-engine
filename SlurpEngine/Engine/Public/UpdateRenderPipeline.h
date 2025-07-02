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

        Entity& createEntity(
            std::string&& name,
            const Vector2<int>& position,
            const geometry::Shape& renderShape,
            render::ColorPaletteIdx color,
            bool centerPosition
        );
        void process(const render::GraphicsBuffer& buffer, float dt);

    private:
        //TODO: have this be a pipeline for entity containers
        std::deque<Entity> _pipeline; // TODO: replace with a priority queue when we want priority
        const render::ColorPalette _colorPalette;
    };
}
