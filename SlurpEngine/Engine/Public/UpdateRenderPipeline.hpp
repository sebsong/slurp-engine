#pragma once
#include <queue>

namespace render
{
    struct GraphicsBuffer;
    struct ColorPalette;
}

namespace slurp
{
    struct Entity;
    struct Tilemap;

    class UpdateRenderPipeline
    {
    public:
        UpdateRenderPipeline(const render::ColorPalette& colorPalette);
        void push(Entity& entity);
        void process(
            const Tilemap& tilemap,
            float dt,
            const render::GraphicsBuffer& buffer
        );

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
        render::ColorPalette _colorPalette;
    };
}
