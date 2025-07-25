#pragma once
#include <queue>

namespace render {
    struct GraphicsBuffer;
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

        void updateAndRender(const render::GraphicsBuffer& buffer, float dt);

    private:
        std::deque<Entity*> _pipeline; // TODO: replace with a priority queue when we want priority
    };
}
