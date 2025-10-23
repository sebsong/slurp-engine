#pragma once

#include "CollectionTypes.h"

#include <queue>

namespace render {
    struct ColorPalette;
}

namespace entity {
    struct Entity;

    typedef uint32_t entity_id;
    const static entity_id INVALID_ENTITY_ID = 0;

    class EntityPipeline {
    public:
        explicit EntityPipeline();

        void registerEntity(Entity& entity);

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

    static void registerEntity(Entity& entity);

    static void initializeEntities();

    static void handleInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState,
        const slurp::GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
    );

    static void updateAndRender(float dt);
}
