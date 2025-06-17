#pragma once
#include <queue>

namespace render {
    struct GraphicsBuffer;
    struct ColorPalette;
}

namespace slurp {
    struct Entity;
    struct Tilemap;

    class UpdateRenderPipeline {
    public:
        explicit UpdateRenderPipeline(const render::ColorPalette& colorPalette);

        void push(Entity& entity);
        void process(const render::GraphicsBuffer& buffer, float dt) const;

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
        const render::ColorPalette _colorPalette;
    };
}
