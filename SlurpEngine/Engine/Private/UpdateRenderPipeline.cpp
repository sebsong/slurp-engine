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

    void UpdateRenderPipeline::push(Entity& entity) {
        _pipeline.push_back(&entity);
    }

    void UpdateRenderPipeline::process(const render::GraphicsBuffer& buffer, float dt) const {
        for (Entity* entity: _pipeline) {
            //TODO: handle destruction
            if (entity->enabled) {
                if (!entity->isStatic) {
                    update::updatePosition(*entity, _pipeline, dt);
                }
                render::drawEntity(buffer, *entity, _colorPalette);
#if DEBUG
                if (entity->drawDebugCollisionShape) {
                    const Vector2<int>& collisionOffset = Vector2<int>::Unit * entity->collisionSquare.radius;
                    render::drawRectBorder(
                        buffer,
                        entity->position - collisionOffset,
                        entity->position + collisionOffset,
                        1,
                        DEBUG_DRAW_COLOR
                    );
                }
#endif
            }
        }
    }
}
