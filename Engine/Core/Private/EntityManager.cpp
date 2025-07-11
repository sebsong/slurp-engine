#include "EntityManager.h"

#include "Entity.h"
#include "Input.h"
#include "Update.h"
#include "Render.h"

namespace slurp {
    EntityManager::EntityManager() : _pipeline(std::deque<Entity*>()) {}

    void EntityManager::registerEntity(Entity& entity) {
        uint32_t id = _pipeline.size();
        _pipeline.emplace_back(&entity);
        entity.id = id;
    }

    void EntityManager::handleInput(
        const MouseState& mouseState,
        const KeyboardState& keyboardState,
        const GamepadState (&controllerStates)[MAX_NUM_CONTROLLERS]
    ) const {
        for (const Entity* entity: _pipeline) {
            if (entity->enabled && entity->handleInput) {
                entity->handleInput(mouseState, keyboardState, controllerStates);
            }
        }
    }

    void EntityManager::updateAndRender(const render::GraphicsBuffer& buffer, float dt) {
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
