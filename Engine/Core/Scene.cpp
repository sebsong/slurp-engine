#include "Scene.h"

#include "Debug.h"
#include "EntityPipeline.h"
#include "Render.h"
#include "Settings.h"
#include "Update.h"

#define MAX_NUM_SCENES 20

namespace scene {
    static Scene* AllScenes[MAX_NUM_SCENES];
    static uint32_t NumRegisteredScenes = 0;

    void registerScene(Scene* scene) {
        AllScenes[NumRegisteredScenes++] = scene;
        scene->isActive = false;
        scene->isPaused = false;
        scene->shouldLoad = false;
        scene->shouldUnload = false;
    }

    void registerEntity(Scene* scene, entity::Entity* entity) {
        scene->entities.push_back(entity);
        entity->scene = scene;
        if (scene->isActive) {
            if (!entity->initialized) {
                entity->initialize();
            }
        }
    }

    void start(Scene* scene) {
        if (scene->isActive) {
            return;
        }

        scene->isPaused = false;
        scene->shouldLoad = true;
        scene->shouldUnload = false;
    }

    void end(Scene* scene) {
        if (!scene->isActive) {
            return;
        }

        scene->shouldUnload = true;
    }

    void pause(Scene* scene) {
        scene->isPaused = true;
    }

    void resume(Scene* scene) {
        scene->isPaused = false;
    }

    void transition(Scene* currentScene, Scene* newScene) {
        end(currentScene);
        start(newScene);
    }

    void handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[4]
    ) {
        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            if (!scene->isActive || scene->isPaused) {
                continue;
            }

            for (entity::Entity* entity: scene->entities) {
                //TODO: handle destruction
                if (!entity->enabled) {
                    continue;
                }
                entity->handleMouseAndKeyboardInput(mouseState, keyboardState);

                for (uint8_t gamepadIndex = 0; gamepadIndex < MAX_NUM_GAMEPADS; gamepadIndex++) {
                    if (!gamepadStates[gamepadIndex].isConnected) { continue; }
                    entity->handleGamepadInput(gamepadIndex, gamepadStates[gamepadIndex]);
                }
            }
        }
    }

    void updateAndRenderEntities(float dt) {
        struct RenderComponent {
            slurp::Vec2<float>* position;
            render::SpriteInstance* sprite;
        };

        uint32_t numEntities = 0;
        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            if (scene->isActive && !scene->isPaused) {
                numEntities += scene->entities.size();
            }
        }

        types::vector_arena<entity::Entity*, memory::SingleFrameArenaAllocator<entity::Entity*> > entitiesToUpdate;
        entitiesToUpdate.reserve(numEntities);

        // TODO: have persistent collection of render components
        types::vector_arena<RenderComponent, memory::SingleFrameArenaAllocator<RenderComponent> > renderComponents;
        renderComponents.reserve(numEntities);

        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            if (!scene->isActive) {
                continue;
            }

            for (entity::Entity* entity: scene->entities) {
                //TODO: handle destruction
                if (entity->enabled) {
                    if (!entity->initialized) {
                        entity->initialize();
                    }

                    if (!scene->isPaused) {
                        entity->update(dt);
                        entity->updatePhysics(dt);
                        entitiesToUpdate.push_back(entity);
                    }

                    const render::RenderInfo& renderInfo = entity->renderInfo;
                    if (renderInfo.numSprites == 0 || !renderInfo.sprites) {
                        continue;
                    }

                    for (int i = 0; i < renderInfo.numSprites; i++) {
                        renderComponents.push_back(
                            RenderComponent{
                                &entity->physicsInfo.position,
                                &renderInfo.sprites[i]
                            }
                        );
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
        }

        for (entity::Entity* entity: entitiesToUpdate) {
            // TODO: move to a separate physics update that potentially runs more frequently
            // TODO: move to using box2d so we don't need to pass all entities
            update::updatePosition(entity, entitiesToUpdate, dt);
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
            }
        }

        std::sort(
            renderComponents.begin(),
            renderComponents.end(),
            [](const RenderComponent& a, const RenderComponent& b) {
                return b.sprite->zOrder < a.sprite->zOrder;
            }
        );
        for (const RenderComponent& component: renderComponents) {
            render::draw(component.position, component.sprite);
        }
    }

    static void load(Scene* scene) {
        scene->load();
        for (entity::Entity* entity: scene->entities) {
            if (!entity->initialized) {
                entity->initialize();
            }
        }
        scene->isActive = true;
    }

    static void unload(Scene* scene) {
        scene->unload();
        scene->entities.clear();
        scene->isActive = false;
    }

    void updateScenes() {
        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            if (scene->shouldUnload) {
                unload(scene);
                scene->shouldUnload = false;
            }
            if (scene->shouldLoad) {
                load(scene);
                scene->shouldLoad = false;
            }
        }
    }
}
