#pragma once
#include "Entity.h"

namespace scene {
    struct Scene {
        bool isActive;
        bool isPaused;
        bool shouldLoad;
        bool shouldUnload;
        types::vector_arena<entity::Entity*> entities;

        virtual void load() = 0;

        virtual void unload() = 0;

        virtual void pause() = 0;

        virtual void resume() = 0;

        Scene() = default;

        virtual ~Scene() = default;
    };

    /** Game **/
    void registerScene(Scene* scene);

    void registerEntity(Scene* scene, entity::Entity* entity);

    void start(Scene* scene);

    void end(Scene* scene);

    void pause(Scene* scene);

    void resume(Scene* scene);

    void transition(Scene* currentScene, Scene* newScene);

    /** Engine **/
    void handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    );

    void updateAndRenderEntities(float dt);

    void updateScenes();
}
