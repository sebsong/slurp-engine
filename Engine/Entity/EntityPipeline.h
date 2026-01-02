#pragma once

#include "CollectionTypes.h"

#include <queue>

#include "Input.h"

namespace slurp {
    struct GamepadState;
    struct KeyboardState;
    struct MouseState;
}

namespace entity {
    struct Entity;

    typedef uint32_t entity_id;
    const static entity_id INVALID_ENTITY_ID = 0;

    // TODO: deprecated
    class EntityPipeline {
    public:
        explicit EntityPipeline();

        void registerEntity(Entity* entity);

        void initializeEntities() const;

        void handleInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState,
            const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
        ) const;

        void updateAndRender(float dt);

        void removeEntity(const Entity* entity);

        void clearAll();

    private:
        entity_id _nextEntityId;
        types::vector_arena<Entity*> _pipeline;
    };

    inline void registerEntity(Entity* entity) {
        slurp::Globals->EntityPipeline->registerEntity(entity);
    }

    inline void initializeEntities() {
        slurp::Globals->EntityPipeline->initializeEntities();
    }

    inline void handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    ) {
        slurp::Globals->EntityPipeline->handleInput(mouseState, keyboardState, gamepadStates);
    }

    inline void updateAndRender(float dt) {
        slurp::Globals->EntityPipeline->updateAndRender(dt);
    }

    inline void removeEntity(const Entity* entity) {
        slurp::Globals->EntityPipeline->removeEntity(entity);
    }

    inline void clearAll() {
        slurp::Globals->EntityPipeline->clearAll();
    }
}
