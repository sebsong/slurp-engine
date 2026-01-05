#include "PauseMenu.h"
#include "Game.h"
#include "Platform.h"
#include "ZOrder.h"
#include "SpriteInstance.h"

namespace ui {
    PauseMenu::PauseMenu()
        : Entity(
              "PauseMenu",
              render::RenderInfo{
                  (render::SpriteInstance[2]){
                      render::SpriteInstance(
                          game::Assets->screenCoverSprite,
                          game::MENU_Z + 1
                      ),
                      render::SpriteInstance(
                          game::Assets->pauseMenuSprite,
                          game::MENU_Z
                      )
                  }
              },
              physics::PhysicsInfo{},
              collision::CollisionInfo{}
          ),
          _resumeButton(
              Button(
                  game::Assets->resumeButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  game::BigButtonShape,
                  {0, -15},
                  std::nullopt,
                  [](Button* _) {},
                  [this](Button* _) {
                      scene::end(game::PauseMenuScene);
                      scene::resume(game::GameScene);
                  },
                  [](Button* _) {},
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  },
                  -2,
                  game::MENU_Z - 1
              )
          ),
          _mainMenuButton(
              Button(
                  game::Assets->mainMenuButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  game::BigButtonShape,
                  {0, -55},
                  std::nullopt,
                  [](Button* _) {},
                  [this](Button* _) {
                      scene::end(game::PauseMenuScene);
                      scene::end(game::GameOverScene);
                      scene::end(game::GameScene);
                      scene::start(game::MainMenuScene);
                  },
                  [](Button* _) {},
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  },
                  -2,
                  game::MENU_Z - 1
              )
          ),
          _exitButton(
              Button(
                  game::Assets->exitButtonTextSprite,
                  game::Assets->bigButtonSprite,
                  game::Assets->bigButtonHoverSprite,
                  game::Assets->bigButtonPressSprite,
                  game::BigButtonShape,
                  {0, -95},
                  std::nullopt,
                  [](Button* _) {},
                  [](Button* _) {
                      platform::exit();
                  },
                  [](Button* _) {},
                  [](Button* _) {
                      audio::play(game::Assets->buttonHoverSound);
                  },
                  -2,
                  game::MENU_Z - 1
              )
          ) {}

    void PauseMenu::initialize() {
        Entity::initialize();

        setAlpha(0, .75);

        scene::registerEntity(scene, &_resumeButton);
        scene::registerEntity(scene, &_mainMenuButton);
        scene::registerEntity(scene, &_exitButton);
    }
}
