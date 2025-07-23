#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6
#include "Collision.h"
#include "Math.h"
#include "Physics.h"

namespace slurp {
    struct Entity;
}

namespace update {
    template<typename Container, typename Value>
    concept Iterable = std::ranges::range<Container> && std::is_same_v<std::ranges::range_value_t<Container>, Value>;

    template<typename T>
        requires Iterable<T, slurp::Entity*>
    void updatePosition(slurp::Entity* entity, T& allEntities, float dt) {
        physics::PhysicsInfo& physicsInfo = entity->physicsInfo;
        collision::CollisionInfo& collisionInfo = entity->collisionInfo;

        if (!physicsInfo.physicsEnabled || collisionInfo.isStatic) { return; }

        slurp::Vector2<int> targetPositionUpdate = physicsInfo.getPositionUpdate(dt);
        if (targetPositionUpdate == slurp::Vector2<int>::Zero) { return; }
        if (!collisionInfo.collisionEnabled) { physicsInfo.position += targetPositionUpdate; }

        slurp::Vector2<int> positionUpdate = targetPositionUpdate;
        for (slurp::Entity* otherEntity: allEntities) {
            physics::PhysicsInfo& otherPhysicsInfo = otherEntity->physicsInfo;
            collision::CollisionInfo& otherCollisionInfo = otherEntity->collisionInfo;
            if (!otherEntity->enabled || !otherCollisionInfo.collisionEnabled || otherEntity == entity) { continue; }

            geometry::Shape minkowskiSum = geometry::getMinkowskiSum(
                collisionInfo.shape.shape,
                otherCollisionInfo.shape.shape
            );
            const slurp::Vector2<int> entityOffsetPosition =
                    physicsInfo.position + collisionInfo.shape.offset + entity->collisionInfo.shape.shape.
                    dimensions;
            const slurp::Vector2<int> otherEntityOffsetPosition =
                    otherPhysicsInfo.position + otherCollisionInfo.shape.offset;
            const slurp::Vector2<int> minkowskiMinPoint = otherEntityOffsetPosition;
            const slurp::Vector2<int> minkowskiMaxPoint = minkowskiMinPoint + minkowskiSum.dimensions;
            if (slurp::Vector2<int> targetPosition = entityOffsetPosition + positionUpdate;
                math::inRange(targetPosition.x, minkowskiMinPoint.x, minkowskiMaxPoint.x) &&
                math::inRange(targetPosition.y, minkowskiMinPoint.y, minkowskiMaxPoint.y)
            ) {
                if (!collisionInfo.isTrigger && !otherCollisionInfo.isTrigger) {
                    if (math::inRange(entityOffsetPosition.y, minkowskiMinPoint.y, minkowskiMaxPoint.y)) {
                        int xAxisPositionUpdate = positionUpdate.x;
                        if (entityOffsetPosition.x <= otherEntityOffsetPosition.x) {
                            xAxisPositionUpdate = minkowskiMinPoint.x - entityOffsetPosition.x;
                        } else { xAxisPositionUpdate = minkowskiMaxPoint.x - entityOffsetPosition.x; }
                        if (std::abs(xAxisPositionUpdate) < std::abs(positionUpdate.x)) {
                            positionUpdate.x = xAxisPositionUpdate;
                        }
                    }
                    if (math::inRange(entityOffsetPosition.x, minkowskiMinPoint.x, minkowskiMaxPoint.x)) {
                        int yAxisPositionUpdate = positionUpdate.y;
                        if (entityOffsetPosition.y <= otherEntityOffsetPosition.y) {
                            yAxisPositionUpdate = minkowskiMinPoint.y - entityOffsetPosition.y;
                        } else { yAxisPositionUpdate = minkowskiMaxPoint.y - entityOffsetPosition.y; }
                        if (std::abs(yAxisPositionUpdate) < std::abs(positionUpdate.y)) {
                            positionUpdate.y = yAxisPositionUpdate;
                        }
                    }
                }

                if (!collisionInfo.collidingWith.contains(otherEntity)) {
                    entity->onCollisionEnter(
                        collision::CollisionDetails{
                            otherEntity
                        }
                    );
                }
                if (!otherCollisionInfo.collidingWith.contains(entity)) {
                    otherEntity->onCollisionEnter(
                        collision::CollisionDetails{
                            entity
                        }
                    );
                }
                collisionInfo.collidingWith.insert(otherEntity);
                otherCollisionInfo.collidingWith.insert(entity);
            } else {
                if (collisionInfo.collidingWith.contains(otherEntity)) {
                    entity->onCollisionExit(
                        collision::CollisionDetails{
                            otherEntity
                        }
                    );
                }
                if (otherCollisionInfo.collidingWith.contains(entity)) {
                    otherEntity->onCollisionExit(
                        collision::CollisionDetails{
                            entity
                        }
                    );
                }
                collisionInfo.collidingWith.erase(otherEntity);
                otherCollisionInfo.collidingWith.erase(entity);
            }
        }
        physicsInfo.position += positionUpdate;
    }
}
