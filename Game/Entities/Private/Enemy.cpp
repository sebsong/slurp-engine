#include "Enemy.h"

#include "Random.h"
#include "Settings.h"

namespace enemy {
    static const slurp::Vec2<float> StartPosition = {-CAMERA_WORLD_WIDTH_MAX + 500, 200};
    static const slurp::Vec2 PositionOffset = {100, 0};
    static const slurp::Vec2 CollisionShapeOffset = {0, 2};
    static constexpr int BaseSpeed = 200;
    static constexpr int BaseAcceleration = 400;
    static constexpr float BaseDirectionChangeDelay = 2;
    static constexpr float DirectionChangeDelayDelta = 1.5;
    static constexpr const char* SpriteFileName = "enemy.bmp";

    static const geometry::Shape Shape = {geometry::Rect, {36, 25}};

    Enemy::Enemy(int i): Entity(
        "Enemy" + std::to_string(i),
        render::RenderInfo(
            game::Assets->enemySprite,
            true
        ),
        physics::PhysicsInfo(
            StartPosition + (PositionOffset * i),
            BaseSpeed,
            BaseAcceleration
        ),
        collision::CollisionInfo(
            false,
            false,
            collision::CollisionShape{
                Shape,
                -(game::Assets->enemySprite->dimensions / 2) + CollisionShapeOffset
            }
        )
    ) {}

    static void setRandomDirection(entity::Entity* entity) {
        float randX = random::randomFloat(-1, 1);
        float randY = random::randomFloat(-1, 1);
        entity->physicsInfo.direction = slurp::Vec2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay() {
        float minDelay = BaseDirectionChangeDelay - DirectionChangeDelayDelta;
        float maxDelay = BaseDirectionChangeDelay + DirectionChangeDelayDelta;
        return random::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(entity::Entity* enemy) {
        setRandomDirection(enemy);
        timer::delay(
            getRandomDirectionChangeDelay(),
            [&] { startUpdateEnemyDirection(enemy); }
        );
    }

    void Enemy::initialize() {
        Entity::initialize();

        this->renderInfo = {
            game::Assets->enemySprite,
            true
        };

        // startUpdateEnemyDirection(this); // TODO: re-enable this
    }
}
