#include "PauseMenu.h"

namespace ui {
    PauseMenu::PauseMenu(): Entity(
        "PauseMenu",
        render::RenderInfo{
            (asset::SpriteInstance[2]){
                asset::SpriteInstance(
                    game::Assets->pauseMenuSprite,
                    game::PAUSE_Z
                ),
                asset::SpriteInstance(
                    game::Assets->screenCoverSprite,
                    game::PAUSE_Z + 1
                )
            }
    },
        physics::PhysicsInfo{},
        collision::CollisionInfo{}
    ) {
        enabled = false;
        setAlpha(1, 0.3);
    }
}
