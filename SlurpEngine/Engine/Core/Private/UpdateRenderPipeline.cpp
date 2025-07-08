#include "UpdateRenderPipeline.h"

#include "Core.h"
#include "Update.h"
#include "Render.h"

namespace slurp {
    UpdateRenderPipeline::UpdateRenderPipeline(
        const render::ColorPalette& colorPalette
    ) : _colorPalette(colorPalette) {
        this->_pipeline = std::deque<Entity*>();
    }

    void UpdateRenderPipeline::process(const render::GraphicsBuffer& buffer, float dt) {
        for (Entity* entity: _pipeline) {
            //TODO: handle destruction
            if (entity->enabled) {
                if (!entity->collisionInfo.isStatic) {
                    update::updatePosition(entity, _pipeline, dt);
                }
                render::drawRenderable(buffer, entity);
#if DEBUG
#if DEBUG_DRAW_COLLISION
                const Vector2<int>& offsetPosition = entity->position + entity->collisionInfo.shape.offset;
                render::drawRectBorder(
                    buffer,
                    offsetPosition,
                    offsetPosition + entity->collisionInfo.shape.shape.dimensions,
                    1,
                    DEBUG_DRAW_COLOR
                );
#endif
#endif
            }
        }
    }
}
