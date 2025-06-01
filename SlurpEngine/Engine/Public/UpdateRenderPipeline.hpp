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
        void push(Entity& entity);
        void process(
            const Tilemap& tilemap,
            float dt,
            const render::GraphicsBuffer& buffer,
            const render::ColorPalette& colorPalette
        );

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
    };
}
