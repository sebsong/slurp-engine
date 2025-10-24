#include "EntityPipeline.h"

#include "Global.h"
#include "Entity.h"
#include "Input.h"
#include "Update.h"
#include "Render.h"

namespace entity {
    EntityPipeline::EntityPipeline() : _nextEntityId(1), _pipeline(types::deque_arena<Entity*>()) {}

    void EntityPipeline::registerEntity(Entity& entity) {
        _pipeline.emplace_back(&entity);
        entity.id = _nextEntityId++;
    }

    void EntityPipeline::initializeEntities() const {
        for (Entity* entity: _pipeline) {
            entity->initialize();
        }
    }

    void EntityPipeline::handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
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

    void EntityPipeline::updateAndRender(float dt) {
        for (Entity* entity: _pipeline) {
            //TODO: handle destruction
            if (entity->enabled) {
                entity->update(dt);
                entity->updatePhysics(dt); // TODO: move to a separate physics update
                update::updatePosition(entity, _pipeline, dt);
                render::drawRenderable(entity->renderInfo, entity->physicsInfo.position);
#if DEBUG
#if DEBUG_DRAW_COLLISION
                const slurp::Vec2<float>& offsetPosition = entity->physicsInfo.position + entity->collisionInfo.shape.offset;
                debug::drawRectBorder(
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

    void registerEntity(Entity& entity) {
        slurp::Globals->EntityPipeline->registerEntity(entity);
    }

    void initializeEntities() {
        slurp::Globals->EntityPipeline->initializeEntities();
    }

    void handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    ) {
        slurp::Globals->EntityPipeline->handleInput(mouseState, keyboardState, gamepadStates);
    }

    void updateAndRender(float dt) {
        slurp::Globals->EntityPipeline->updateAndRender(dt);
    }
}
