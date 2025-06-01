#include <UpdateRenderPipeline.hpp>

#include <Core.hpp>
#include <Update.hpp>
#include <Render.hpp>

namespace slurp
{
    void UpdateRenderPipeline::push(Entity& entity)
    {
        _pipeline.push_back(&entity);
    }

    void UpdateRenderPipeline::process(
        const Tilemap& tilemap,
        float dt,
        const render::GraphicsBuffer& buffer,
        const render::ColorPalette& colorPalette
    )
    {
        for (Entity* entity : _pipeline)
        {
            //TODO: handle destruction
            if (entity->enabled)
            {
                update::updatePosition(*entity, tilemap, dt);
                render::drawEntity(buffer, *entity, colorPalette);
            }
        }
    }
}
