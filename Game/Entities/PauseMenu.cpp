#include "PauseMenu.h"
#include "Game.h"
#include "Platform.h"
#include "ZOrder.h"
#include "Sprite.h"

namespace ui {
    const geometry::Shape& PauseButtonShape = geometry::Shape(geometry::Rect, {52, 34});

    PauseMenu::PauseMenu()
        : Entity(
              "PauseMenu",
              render::RenderInfo{
                  (asset::SpriteInstance[2]){
                      asset::SpriteInstance(
                          game::Assets->screenCoverSprite,
                          game::PAUSE_Z + 1
                      ),
                      asset::SpriteInstance(
                          game::Assets->pauseMenuSprite,
                          game::PAUSE_Z
                      )
                  }
              },
              physics::PhysicsInfo{},
              collision::CollisionInfo{}
          ),
          _resumeButton(
              UIButton(
                  game::Assets->resumeButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  PauseButtonShape,
                  {0, 25},
                  std::nullopt,
                  [](UIButton* _) {},
                  [this](UIButton* _) {
                      toggle();
                  },
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      audio::play(game::Assets->buttonHover);
                  },
                  -2,
                  game::PAUSE_Z - 1
              )
          ),
          _exitButton(
              UIButton(
                  game::Assets->exitButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  PauseButtonShape,
                  {0, -25},
                  std::nullopt,
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      platform::exit();
                  },
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      audio::play(game::Assets->buttonHover);
                  },
                  -2,
                  game::PAUSE_Z - 1
              )
          ) {
        toggle();
        setAlpha(0, .75); // TODO: fix alpha z sorting
    }

    void PauseMenu::toggle() {
        if (enabled) {
            enabled = false;
            _resumeButton.enabled = false;
            _exitButton.enabled = false;
        } else {
            enabled = true;
            _resumeButton.enabled = true;
            _exitButton.enabled = true;
        }
    }
}
