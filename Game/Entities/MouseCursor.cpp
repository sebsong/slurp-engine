#include "MouseCursor.h"

#include "EntityPipeline.h"
#include "Game.h"

namespace mouse_cursor {
    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            asset::SpriteInstance(
                game::Assets->mouseCursorSprite,
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
