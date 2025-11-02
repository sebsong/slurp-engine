#pragma once

#include "CollectionTypes.h"

#include <queue>

namespace entity {
    struct Entity;

    typedef uint32_t entity_id;
    const static entity_id INVALID_ENTITY_ID = 0;

    class EntityPipeline {
    public:
        explicit EntityPipeline();

        void registerEntity(Entity& entity);

        Entity* hitTest(const slurp::Vec2<float>& location) const;

        void initializeEntities() const;

        void handleInput(
            const slurp::MouseState& mouseState,
            const slurp::KeyboardState& keyboardState,
            const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
        ) const;

        void updateAndRender(float dt);

    private:
        entity_id _nextEntityId;
        types::deque_arena<Entity*> _pipeline;
    };

    inline Entity* hitTest(const slurp::Vec2<float>& location) {
        return slurp::Globals->EntityPipeline->hitTest(location);
    };

    inline void registerEntity(Entity& entity) {
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
}
