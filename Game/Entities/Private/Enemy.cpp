#include "Enemy.h"

#include "Random.h"

namespace game {
    static const slurp::Vector2 EnemyStartPos = {400, 200};
    static const slurp::Vector2 EnemyPosOffset = {100, 0};
    static constexpr int BaseEnemySizePixels = 17;
    static constexpr int BaseEnemySpeed = 200;
    static constexpr float BaseEnemyDirectionChangeDelay = 2;
    static constexpr float EnemyDirectionChangeDelayDelta = 1.5;
    static constexpr const char* EnemySpriteFileName = "enemy.bmp";
    static const render::Sprite EnemySprite = render::loadSprite(EnemySpriteFileName);

    const geometry::Shape enemyShape = {geometry::Rect, {BaseEnemySizePixels, BaseEnemySizePixels}};

    Enemy::Enemy(int i): Entity(
        "Enemy" + std::to_string(i),
        render::RenderInfo(
            EnemySprite,
            true
        ),
        physics::PhysicsInfo(
            EnemyStartPos + (EnemyPosOffset * i),
            BaseEnemySpeed
        ),
        collision::CollisionInfo(
            false,
            false,
            enemyShape,
            true
        )
    ) {}

    static void setRandomDirection(slurp::Entity* entity) {
        float randX = random::randomFloat(-1, 1);
        float randY = random::randomFloat(-1, 1);
        entity->physicsInfo.direction = slurp::Vector2<float>(randX, randY).normalize();
    }

    static float getRandomDirectionChangeDelay() {
        float minDelay = BaseEnemyDirectionChangeDelay - EnemyDirectionChangeDelayDelta;
        float maxDelay = BaseEnemyDirectionChangeDelay + EnemyDirectionChangeDelayDelta;
        return random::randomFloat(minDelay, maxDelay);
    }

    static void startUpdateEnemyDirection(slurp::Entity* enemy) {
        setRandomDirection(enemy);
        timer::delay(
            getRandomDirectionChangeDelay(),
            [&] { startUpdateEnemyDirection(enemy); }
        );
    }

    void Enemy::initialize() {
        Entity::initialize();

        // startUpdateEnemyDirection(this); // TODO: re-enable this
    }
}
