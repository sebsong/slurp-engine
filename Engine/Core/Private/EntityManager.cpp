#include "EntityManager.h"

#include "Entity.h"
#include "Input.h"
#include "Update.h"
#include "Render.h"

namespace slurp {
    EntityManager::EntityManager(
        const render::RenderApi* renderApi
    ) : _pipeline(std::deque<Entity*>()), _renderApi(renderApi) {}

    void EntityManager::registerEntity(Entity& entity) {
        uint32_t id = _pipeline.size();
        _pipeline.emplace_back(&entity);
        entity.id = id;
    }

    void EntityManager::initialize() const { for (Entity* entity: _pipeline) { entity->initialize(); } }

    void EntityManager::handleInput(
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    ) const {
        for (Entity* entity: _pipeline) {
            if (!entity->enabled) { continue; }
            entity->handleMouseAndKeyboardInput(mouseState, keyboardState);

            for (uint8_t gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
                if (!gamepadStates[gamepadIndex].isConnected) { continue; }
                entity->handleGamepadInput(gamepadIndex, gamepadStates[gamepadIndex]);
            }
        }
    }

    void EntityManager::updateAndRender(const render::GraphicsBuffer& buffer, float dt) {
        for (Entity* entity: _pipeline) {
            //TODO: handle destruction
            if (entity->enabled) {
                entity->update(dt);
                entity->updatePhysics(dt); // TODO: move to a separate physics update
                update::updatePosition(entity, _pipeline, dt);
                if (entity->renderInfo.openGLInfo.elementCount > 0) {
                    render::drawRenderable(entity->renderInfo, entity->physicsInfo.position, _renderApi);
                } else {
                    render::drawRenderable(buffer, entity->renderInfo, entity->physicsInfo.position);
                }
#if DEBUG
#if DEBUG_DRAW_COLLISION
                const Vec2<float>& offsetPosition = entity->physicsInfo.position + entity->collisionInfo.shape.offset;
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
