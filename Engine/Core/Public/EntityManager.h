#pragma once

#include "CollectionTypes.h"

#include <queue>

namespace render {
    struct ColorPalette;
}

namespace slurp {
    struct Entity;

    class EntityManager {
    public:
        explicit EntityManager();

        void registerEntity(Entity& entity);

        void initialize() const;

        void handleInput(
            const MouseState& mouseState,
            const KeyboardState& keyboardState,
            const GamepadState (&gamepadStates)[MAX_NUM_GAMEPADS]
        ) const;

        void updateAndRender(float dt);

    private:
        types::deque_arena<Entity*> _pipeline;
    };
}
