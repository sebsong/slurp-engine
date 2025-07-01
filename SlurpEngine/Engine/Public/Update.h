#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6
#include "Collision.h"
#include "Math.h"

namespace slurp {
    struct Entity;
}

namespace update {
    template<typename Container, typename Value>
    concept Iterable = std::ranges::range<Container> && std::is_same_v<std::ranges::range_value_t<Container>, Value>;

    template<typename T>
        requires Iterable<T, slurp::Entity>
    void updatePosition(slurp::Entity& entity, T& allEntities, float dt) {
        slurp::Vector2<int> targetPositionUpdate = (entity.direction * entity.speed * dt);
        if (targetPositionUpdate == slurp::Vector2<int>::Zero) {
            return;
        }
        collision::CollisionInfo& collisionInfo = entity.collisionInfo;
        if (!collisionInfo.collisionEnabled) {
            entity.position += targetPositionUpdate;
        }

        slurp::Vector2<int> positionUpdate = targetPositionUpdate;
        for (slurp::Entity& otherEntity: allEntities) {
            collision::CollisionInfo& otherCollisionInfo = otherEntity.collisionInfo;
            if (!otherCollisionInfo.collisionEnabled || otherEntity == entity) {
                continue;
            }

            geometry::Shape minkowskiSum = geometry::getMinkowskiSum(
                collisionInfo.shape.shape,
                otherCollisionInfo.shape.shape
            );
            // TODO: account for collision offset here
            const slurp::Vector2<int> entityOffsetPosition = entity.position + entity.collisionInfo.shape.shape.dimensions - collisionInfo.shape.offset;
            // const slurp::Vector2<int> otherEntityOffsetPosition = otherEntity.position - otherCollisionInfo.shape.shape.dimensions - collisionInfo.shape.offset;
            const slurp::Vector2<int> minkowskiMinPoint = otherEntity.position - otherCollisionInfo.shape.offset;
            const slurp::Vector2<int> minkowskiMaxPoint = minkowskiMinPoint + minkowskiSum.dimensions;
            int minkowskiMinX = minkowskiMinPoint.x;
            int minkowskiMaxX = minkowskiMaxPoint.x;
            int minkowskiMinY = minkowskiMinPoint.y;
            int minkowskiMaxY = minkowskiMaxPoint.y;
            if (slurp::Vector2<int> targetPosition = entityOffsetPosition + positionUpdate;
                math::inRange(targetPosition.x, minkowskiMinX, minkowskiMaxX) &&
                math::inRange(targetPosition.y, minkowskiMinY, minkowskiMaxY)
            ) {
                if (math::inRange(entityOffsetPosition.y, minkowskiMinY, minkowskiMaxY)) {
                    int xAxisPositionUpdate = positionUpdate.x;
                    if (entityOffsetPosition.x <= otherEntity.position.x) {
                        xAxisPositionUpdate = minkowskiMinX - entityOffsetPosition.x;
                    } else {
                        xAxisPositionUpdate = minkowskiMaxX - entityOffsetPosition.x;
                    }
                    if (std::abs(xAxisPositionUpdate) < std::abs(positionUpdate.x)) {
                        positionUpdate.x = xAxisPositionUpdate;
                    }
                }
                if (math::inRange(entityOffsetPosition.x, minkowskiMinX, minkowskiMaxX)) {
                    int yAxisPositionUpdate = positionUpdate.y;
                    if (entityOffsetPosition.y <= otherEntity.position.y) {
                        yAxisPositionUpdate = minkowskiMinY - entityOffsetPosition.y;
                    } else {
                        yAxisPositionUpdate = minkowskiMaxY - entityOffsetPosition.y;
                    }
                    if (std::abs(yAxisPositionUpdate) < std::abs(positionUpdate.y)) {
                        positionUpdate.y = yAxisPositionUpdate;
                    }
                }
                if (collisionInfo.onCollision && !collisionInfo.collidingWith.contains(&otherEntity)) {
                    collisionInfo.onCollision(otherEntity);
                }
                if (otherCollisionInfo.onCollision && !otherCollisionInfo.collidingWith.contains(&entity)) {
                    otherCollisionInfo.onCollision(entity);
                }
                collisionInfo.collidingWith.insert(&otherEntity);
                otherCollisionInfo.collidingWith.insert(&entity);
            } else {
                collisionInfo.collidingWith.erase(&otherEntity);
                otherCollisionInfo.collidingWith.erase(&entity);
            }
        }
        entity.position += positionUpdate;
    }
}
