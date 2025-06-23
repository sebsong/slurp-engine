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
        slurp::Vector2<int> targetPosition = entity.position + targetPositionUpdate;
        collision::CollisionInfo& collisionInfo = entity.collisionInfo;
        if (!collisionInfo.collisionEnabled) {
            entity.position += targetPositionUpdate;
        }

        bool didCollide = false;
        slurp::Vector2<int> positionUpdate = targetPositionUpdate;
        for (slurp::Entity& otherEntity: allEntities) {
            collision::CollisionInfo& otherCollisionInfo = otherEntity.collisionInfo;
            if (!otherCollisionInfo.collisionEnabled || otherEntity == entity) {
                continue;
            }

            collision::CollisionSquare minkowskiSum = collision::getMinkowskiSum(
                collisionInfo.collisionSquare, otherCollisionInfo.collisionSquare);
            int minkowskiMinX = otherEntity.position.x - minkowskiSum.radius;
            int minkowskiMaxX = otherEntity.position.x + minkowskiSum.radius;
            int minkowskiMinY = otherEntity.position.y - minkowskiSum.radius;
            int minkowskiMaxY = otherEntity.position.y + minkowskiSum.radius;
            if (
                math::inRange(targetPosition.x, minkowskiMinX, minkowskiMaxX) &&
                math::inRange(targetPosition.y, minkowskiMinY, minkowskiMaxY)
            ) {
                didCollide = true;
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
                // TODO: need to track which entities have already collided,
                // TODO(cont): the current check means we can't have multiple collisions
                if (collisionInfo.onCollision && collisionInfo.collisionState != collision::CollisionState::Colliding) {
                    collisionInfo.onCollision(otherEntity);
                }
                if (otherCollisionInfo.onCollision && otherCollisionInfo.collisionState != collision::CollisionState::Colliding) {
                    otherCollisionInfo.onCollision(entity);
                }
                collisionInfo.collisionState = collision::CollisionState::Colliding;
                otherCollisionInfo.collisionState = collision::CollisionState::Colliding;
            }
        }
        if (!didCollide) {
            collisionInfo.collisionState = collision::CollisionState::None;
        }
        entity.position += positionUpdate;
    }
}
