#include "MouseCursor.h"

#include "EntityPipeline.h"
#include "Game.h"

namespace mouse_cursor {
    MouseCursor::MouseCursor(): Entity(
        "MouseCursor",
        render::RenderInfo(
            game::Assets->mouseCursorSprite,
            true,
            game::MOUSE_Z
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

        if (mouseState.justPressed(slurp::MouseCode::LeftClick)) {
            Entity* entity = entity::hitTest(mouseState.position);
            if (entity) {
                logging::debug(
                    std::format(
                        "HIT: {}",
                        entity->name
                    )
                );
            } else {
                logging::debug("HIT: None");
            }
        }
    }
}
