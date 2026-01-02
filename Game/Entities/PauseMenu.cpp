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
                          game::PauseAssets->screenCoverSprite,
                          game::PAUSE_Z + 1
                      ),
                      render::SpriteInstance(
                          game::PauseAssets->pauseMenuSprite,
                          game::PAUSE_Z
                      )
                  }
              },
              physics::PhysicsInfo{},
              collision::CollisionInfo{}
          ),
          _resumeButton(
              UIButton(
                  game::PauseAssets->resumeButtonTextSprite,
                  game::PauseAssets->bigButtonSprite,
                  game::PauseAssets->bigButtonHoverSprite,
                  game::PauseAssets->bigButtonPressSprite,
                  PauseButtonShape,
                  {0, 25},
                  std::nullopt,
                  [](UIButton* _) {},
                  [this](UIButton* _) {
                      scene::end(game::PauseState);
                  },
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      audio::play(game::PauseAssets->buttonHover);
                  },
                  -2,
                  game::PAUSE_Z - 1
              )
          ),
          _exitButton(
              UIButton(
                  game::PauseAssets->exitButtonTextSprite,
                  game::PauseAssets->bigButtonSprite,
                  game::PauseAssets->bigButtonHoverSprite,
                  game::PauseAssets->bigButtonPressSprite,
                  PauseButtonShape,
                  {0, -25},
                  std::nullopt,
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      platform::exit();
                  },
                  [](UIButton* _) {},
                  [](UIButton* _) {
                      audio::play(game::PauseAssets->buttonHover);
                  },
                  -2,
                  game::PAUSE_Z - 1
              )
          ) {
    }

    void PauseMenu::initialize() {
        Entity::initialize();

        setAlpha(0, .75);

        scene::registerEntity(game::PauseState, &_resumeButton);
        scene::registerEntity(game::PauseState, &_exitButton);
    }
}
