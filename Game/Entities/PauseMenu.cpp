#include "PauseMenu.h"
#include "Game.h"
#include "Platform.h"
#include "ZOrder.h"
#include "SpriteInstance.h"

namespace ui {
    const geometry::Shape& PauseButtonShape = geometry::Shape(geometry::Rect, {52, 34});

    PauseMenu::PauseMenu()
        : Entity(
              "PauseMenu",
              render::RenderInfo{
                  (render::SpriteInstance[2]){
                      render::SpriteInstance(
                          game::Assets->screenCoverSprite,
                          game::PAUSE_Z + 1
                      ),
                      render::SpriteInstance(
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
                  game::Assets->playButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  PauseButtonShape,
                  {0, 25},
                  std::nullopt,
                  [](UIButton* _) {},
                  [this](UIButton* _) {
                      scene::end(game::PauseMenuScene);
                  },
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      audio::play(game::Assets->buttonHoverSound);
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
                      audio::play(game::Assets->buttonHoverSound);
                  },
                  -2,
                  game::PAUSE_Z - 1
              )
          ) {}

    void PauseMenu::initialize() {
        Entity::initialize();

        setAlpha(0, .75);

        scene::registerEntity(game::PauseMenuScene, &_resumeButton);
        scene::registerEntity(game::PauseMenuScene, &_exitButton);
    }
}
