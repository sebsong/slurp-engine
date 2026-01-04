#include "GameOverScreen.h"

#include "Game.h"

namespace ui {
    GameOverScreen::GameOverScreen()
        : Entity(
              "PauseMenu",
              render::RenderInfo{
                  (render::SpriteInstance[2]){
                      render::SpriteInstance(
                          game::Assets->screenCoverSprite,
                          game::PAUSE_Z + 1
                      ),
                      render::SpriteInstance(
                          game::Assets->gameOverScreenSprite,
                          game::PAUSE_Z
                      )
                  }
              },
              physics::PhysicsInfo{},
              collision::CollisionInfo{}
          ) {}

    void GameOverScreen::initialize() {
        Entity::initialize();

        setAlpha(0, .75);
    }
}
