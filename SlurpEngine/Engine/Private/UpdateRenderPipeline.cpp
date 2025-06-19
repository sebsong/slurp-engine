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
        int size,
        const Vector2<int>& position,
        render::ColorPaletteIdx color,
        bool centerPosition
    ) {
        _pipeline.emplace_back();
        Entity& entity = _pipeline.back();
        entity.id = _pipeline.size() - 1;
        entity.name = std::move(name);
        entity.enabled = true;
        entity.size = size;
        entity.position = position;
        if (centerPosition) {
            entity.renderOffset = Vector2<int>::Unit * entity.size / 2;
        }
        entity.color = color;
        return entity;
    }

    void UpdateRenderPipeline::process(const render::GraphicsBuffer& buffer, float dt) {
        for (Entity& entity: _pipeline) {
            //TODO: handle destruction
            if (entity.enabled) {
                if (!entity.isStatic) {
                    update::updatePosition(entity, _pipeline, dt);
                }
                render::drawEntity(buffer, entity, _colorPalette);
#if DEBUG
                if (entity.drawDebugCollisionShape) {
                    const Vector2<int>& collisionOffset = Vector2<int>::Unit * entity.collisionSquare.radius;
                    render::drawRectBorder(
                        buffer,
                        entity.position - collisionOffset,
                        entity.position + collisionOffset,
                        1,
                        DEBUG_DRAW_COLOR
                    );
                }
#endif
            }
        }
    }
}
