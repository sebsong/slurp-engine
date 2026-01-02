#include "MouseCursor.h"

#include "EntityPipeline.h"
#include "Game.h"

namespace mouse_cursor {
    MouseCursor::MouseCursor(asset::Sprite* sprite): Entity(
        "MouseCursor",
        render::RenderInfo(
            render::SpriteInstance(
                sprite,
                game::MOUSE_Z
            )
        ),
        physics::PhysicsInfo(slurp::Vec2<float>::Zero),
        collision::CollisionInfo()
    ) {}

    void MouseCursor::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);
        this->physicsInfo.position = mouseState.position;
    }
}
