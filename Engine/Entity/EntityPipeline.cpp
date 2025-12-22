#include "EntityPipeline.h"

#include "Entity.h"
#include "Input.h"
#include "Render.h"
#include "Settings.h"
#include "Update.h"

namespace entity {
    EntityPipeline::EntityPipeline() : _nextEntityId(1), _pipeline(types::vector_arena<Entity*>()) {}

    void EntityPipeline::registerEntity(Entity* entity) {
        if (entity->id == INVALID_ENTITY_ID) {
            _pipeline.emplace_back(entity);
            entity->id = _nextEntityId++;
        } else {
            // NOTE: Ensure that ids are in sorted order
            _pipeline.insert(
                std::upper_bound(
                    _pipeline.begin(),
                    _pipeline.end(),
                    entity,
                    [](const Entity* a, const Entity* b) { return a->id < b->id; }
                    // TODO: move this out to a comparator
                ),
                entity
            );
        }
    }

    void EntityPipeline::initializeEntities() const {
        for (Entity* entity: _pipeline) {
            if (entity->enabled && !entity->initialized) {
                entity->initialize();
            }
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

                if (!entity->initialized) {
                    entity->initialize();
                }

                entity->update(dt);
                entity->updatePhysics(dt);
                // TODO: move to a separate physics update that potentially runs more frequently
                update::updatePosition(entity, _pipeline, dt);

                const render::RenderInfo& renderInfo = entity->renderInfo;
                if (renderInfo.numSprites == 0 || !renderInfo.sprites) {
                    continue;
                }

                for (int i = 0; i < renderInfo.numSprites; i++) {
                    render::SpriteInstance& sprite = renderInfo.sprites[i];
                    if (sprite.syncZOrderToY) {
                        sprite.zOrder = static_cast<int>(
                            (entity->physicsInfo.position.y / WORLD_HEIGHT_MAX) * Z_ORDER_MAX
                        );
                    }
                    asset::SpriteAnimation& animation = sprite.animation;
                    if (animation.isPlaying) {
                        animation.update(dt);
                    }

                    render::draw(entity->physicsInfo.position, sprite, animation);
                }
#if DEBUG
#if DEBUG_DRAW_COLLISION
                if (entity->collisionInfo.collisionEnabled) {
                    const slurp::Vec2<float>& offsetPosition =
                            entity->physicsInfo.position + entity->collisionInfo.shape.offset;
                    debug::drawRectBorder(
                        offsetPosition,
                        offsetPosition + entity->collisionInfo.shape.shape.dimensions,
                        1,
                        DEBUG_DRAW_COLOR
                    );
                }
#endif
#endif
            }
        }
    }

    void EntityPipeline::removeEntity(const Entity* entity) {
        types::vector_arena<Entity*>::iterator searchBegin = std::lower_bound(
            _pipeline.begin(),
            _pipeline.end(),
            entity,
            [](const Entity* a, const Entity* b) { return a->id < b->id; }
        );
        types::vector_arena<Entity*>::iterator position = std::find(searchBegin, _pipeline.end(), entity);
        ASSERT_LOG(
            position != _pipeline.end(),
            std::format("Could not find entity to remove from pipeline: {}", entity->name)
        );
        _pipeline.erase(position);
    }

    void EntityPipeline::clearAll() {
        _pipeline.clear();
    }
}
