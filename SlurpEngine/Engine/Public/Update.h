#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6
#include "Math.h"

namespace slurp {
    struct Entity;
}

namespace update {
    template<typename Container, typename Value>
    concept Iterable = std::ranges::range<Container> && std::is_same_v<std::ranges::range_value_t<Container>, Value>;

    template<typename T>
        requires Iterable<T, slurp::Entity>
    void updatePosition(slurp::Entity& entity, const T& allEntities, float dt) {
        slurp::Vector2<int> targetPositionUpdate = (entity.direction * entity.speed * dt);
        slurp::Vector2<int> targetPosition = entity.position + targetPositionUpdate;
        if (!entity.collisionEnabled) {
            entity.position += targetPositionUpdate;
        }
        slurp::Vector2<int> positionUpdate = targetPositionUpdate;
        for (const slurp::Entity& otherEntity: allEntities) {
            if (!otherEntity.collisionEnabled || otherEntity == entity) {
                continue;
            }

            collision::CollisionSquare minkowskiSum = collision::getMinkowskiSum(
                entity.collisionSquare, otherEntity.collisionSquare);
            int minkowskiMinX = otherEntity.position.x - minkowskiSum.radius;
            int minkowskiMaxX = otherEntity.position.x + minkowskiSum.radius;
            int minkowskiMinY = otherEntity.position.y - minkowskiSum.radius;
            int minkowskiMaxY = otherEntity.position.y + minkowskiSum.radius;
            if (
                math::inRange(targetPosition.x, minkowskiMinX, minkowskiMaxX) &&
                math::inRange(targetPosition.y, minkowskiMinY, minkowskiMaxY)
            ) {
                if (math::inRange(entity.position.y, minkowskiMinY, minkowskiMaxY)) {
                    int xAxisPositionUpdate = positionUpdate.x;
                    if (entity.position.x < otherEntity.position.x) {
                        xAxisPositionUpdate = minkowskiMinX - entity.position.x;
                    } else {
                        xAxisPositionUpdate = minkowskiMaxX - entity.position.x;
                    }
                    if (std::abs(xAxisPositionUpdate) < std::abs(positionUpdate.x)) {
                        positionUpdate.x = xAxisPositionUpdate;
                    }
                }
                if (math::inRange(entity.position.x, minkowskiMinX, minkowskiMaxX)) {
                    int yAxisPositionUpdate = positionUpdate.y;
                    if (entity.position.y < otherEntity.position.y) {
                        yAxisPositionUpdate = minkowskiMinY - entity.position.y;
                    } else {
                        yAxisPositionUpdate = minkowskiMaxY - entity.position.y;
                    }
                    if (std::abs(yAxisPositionUpdate) < std::abs(positionUpdate.y)) {
                        positionUpdate.y = yAxisPositionUpdate;
                    }
                }
                entity.position += positionUpdate;
                if (entity.onCollision) {
                    entity.onCollision(otherEntity);
                }
                return;
            }
        }
        entity.position += targetPositionUpdate;
    }
}
