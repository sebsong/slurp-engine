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

        template<typename T>
            requires std::derived_from<T, Entity>
        T& initAndRegister(
            T& outEntity,
            std::string&& name,
            const Vector2<int>& position,
            const geometry::Shape& renderShape,
            render::ColorPaletteIdx color,
            bool isCentered
        ) {
            new(&outEntity) T(
                std::move(name),
                renderShape,
                isCentered,
                _colorPalette.colors[color],
                position
            );
            registerEntity(outEntity);
            return outEntity;
        }

        void registerEntity(Entity& entity);

        void process(const render::GraphicsBuffer& buffer, float dt);

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
        const render::ColorPalette _colorPalette;
    };
}
