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

        template <typename T>
        requires std::derived_from<T, Entity>
        T& initAndRegister(
            T& outEntity,
            std::string&& name,
            const Vector2<int>& position,
            const geometry::Shape& renderShape,
            render::ColorPaletteIdx color,
            bool centerPosition
        ) {
            new(&outEntity) T();
            uint32_t id = _pipeline.size();
            _pipeline.emplace_back(&outEntity);
            outEntity.id = id;
            outEntity.name = std::move(name);
            outEntity.enabled = true;
            outEntity.position = position;
            outEntity.renderShape.shape = renderShape;
            if (centerPosition) {
                outEntity.renderShape.renderOffset = -renderShape.dimensions / 2;
            }
            assert(color < COLOR_PALETTE_SIZE);
            outEntity.renderShape.color = _colorPalette.colors[color];
            return outEntity;
        }


        void process(const render::GraphicsBuffer& buffer, float dt);

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
        const render::ColorPalette _colorPalette;
    };
}
