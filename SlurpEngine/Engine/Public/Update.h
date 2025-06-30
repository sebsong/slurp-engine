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
            int minkowskiMinX = otherEntity.position.x - minkowskiSum.dimensions.x / 2;
            int minkowskiMaxX = otherEntity.position.x + minkowskiSum.dimensions.x / 2;
            int minkowskiMinY = otherEntity.position.y - minkowskiSum.dimensions.y / 2;
            int minkowskiMaxY = otherEntity.position.y + minkowskiSum.dimensions.y / 2;
            if (slurp::Vector2<int> targetPosition = entity.position + positionUpdate;
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
