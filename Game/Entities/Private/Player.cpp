#include "Player.h"

namespace game {
    static const slurp::Vector2 PlayerStartPos = {640, 360};
    static constexpr int BasePlayerSizePixels = 20;
    static constexpr render::ColorPaletteIdx PlayerColorPalletIdx = 3;
    static constexpr render::ColorPaletteIdx PlayerParryColorPalletIdx = 0;
    static constexpr int BasePlayerSpeed = 400;
    static constexpr int SprintPlayerSpeed = 800;
    static constexpr geometry::Shape playerShape = {
        geometry::Rect,
        {BasePlayerSizePixels, BasePlayerSizePixels}
    };
    static constexpr const char* Name = "Player";

    static constexpr float ParryActiveDuration = .1f;

    Player::Player() {
        uint32_t id;
        std::string name;
        bool enabled;
        render::RenderShape renderShape;
        Vector2<int> position;
        float speed;
        Vector2<float> direction;
        collision::CollisionInfo collisionInfo;
        bool shouldDestroy;

        name = Name;
        enabled = true;
        GlobalUpdateRenderPipeline->initAndRegister(
            GlobalGameState->player,
            "Player",
            PlayerStartPos,
            playerShape,
            PlayerColorPalletIdx,
            true
        );
        GlobalGameState->player.speed = BasePlayerSpeed;
        GlobalGameState->player.enableCollision(
            false,
            playerShape,
            true,
            [](const Entity* otherEntity) {
                std::cout << "ENTER: " << otherEntity->name << std::endl;
            },
            [](const Entity* otherEntity) {
                std::cout << "EXIT: " << otherEntity->name << std::endl;
            }
        );
    }
}
