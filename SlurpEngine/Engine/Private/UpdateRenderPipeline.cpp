#include "UpdateRenderPipeline.h"

#include "Core.h"
#include "Update.h"
#include "Render.h"

namespace slurp {
    UpdateRenderPipeline::UpdateRenderPipeline(
        const render::ColorPalette& colorPalette
    ) : _colorPalette(colorPalette) {
        this->_pipeline = std::deque<Entity>();
    }

    Entity& UpdateRenderPipeline::createEntity(
        std::string&& name,
        const Vector2<int>& position,
        const geometry::Shape& renderShape,
        render::ColorPaletteIdx color,
        bool centerPosition
    ) {
        uint32_t id = _pipeline.size();
        _pipeline.emplace_back();
        Entity& entity = _pipeline.back();
        entity.id = id;
        entity.name = std::move(name);
        entity.enabled = true;
        entity.position = position;
        entity.renderShape.shape = renderShape;
        if (centerPosition) {
            entity.renderShape.renderOffset = renderShape.dimensions / 2;
        }
        assert(color < COLOR_PALETTE_SIZE);
        entity.renderShape.color = _colorPalette.colors[color];
        return entity;
    }

    void UpdateRenderPipeline::process(const render::GraphicsBuffer& buffer, float dt) {
        for (Entity& entity: _pipeline) {
            //TODO: handle destruction
            if (entity.enabled) {
                if (!entity.collisionInfo.isStatic) {
                    update::updatePosition(entity, _pipeline, dt);
                }
                render::drawRenderable(buffer, entity);
#if DEBUG
#if DEBUG_DRAW_COLLISION
                const Vector2<int>& offsetPosition = entity.position - entity.collisionInfo.shape.offset;
                render::drawRectBorder(
                    buffer,
                    offsetPosition,
                    offsetPosition + entity.collisionInfo.shape.shape.dimensions,
                    1,
                    DEBUG_DRAW_COLOR
                );
#endif
#endif
            }
        }
    }
}
