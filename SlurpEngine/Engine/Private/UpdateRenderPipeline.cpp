#include "UpdateRenderPipeline.h"

#include "Core.h"
#include "Update.h"
#include "Render.h"

namespace slurp {
    UpdateRenderPipeline::UpdateRenderPipeline(const render::ColorPalette& colorPalette) {
        this->_pipeline = std::deque<Entity*>();
        this->_colorPalette = colorPalette;
    }

    void UpdateRenderPipeline::push(Entity& entity) {
        _pipeline.push_back(&entity);
    }

    void UpdateRenderPipeline::process(
        const Tilemap& tilemap,
        const float dt,
        const render::GraphicsBuffer& buffer
    ) {
        for (Entity* entity: _pipeline) {
            //TODO: handle destruction
            if (entity->enabled) {
                update::updatePosition(*entity, tilemap, dt);
                render::drawEntity(buffer, *entity, _colorPalette);
            }
        }
    }
}
