#pragma once

#define EMPTY_COLOR_PALETTE_IDX 6
#include "Collision.h"
#include "SlurpMath.h"
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

        slurp::Vec2<float> targetPositionUpdate = physicsInfo.getPositionUpdate(dt);
        if (targetPositionUpdate == slurp::Vec2<float>::Zero) { return; }
        if (!collisionInfo.collisionEnabled) { physicsInfo.position += targetPositionUpdate; }

        slurp::Vec2<float> positionUpdate = targetPositionUpdate;
        for (slurp::Entity* otherEntity: allEntities) {
            physics::PhysicsInfo& otherPhysicsInfo = otherEntity->physicsInfo;
            collision::CollisionInfo& otherCollisionInfo = otherEntity->collisionInfo;
            if (!otherEntity->enabled || !otherCollisionInfo.collisionEnabled || otherEntity == entity) { continue; }

            geometry::Shape minkowskiSum = geometry::getMinkowskiSum(
                collisionInfo.shape.shape,
                otherCollisionInfo.shape.shape
            );
            const slurp::Vec2<float> entityOffsetPosition =
                    physicsInfo.position + collisionInfo.shape.offset + entity->collisionInfo.shape.shape.
                    dimensions;
            const slurp::Vec2<float> otherEntityOffsetPosition =
                    otherPhysicsInfo.position + otherCollisionInfo.shape.offset;
            const slurp::Vec2<float> minkowskiMinPoint = otherEntityOffsetPosition;
            const slurp::Vec2<float> minkowskiMaxPoint = minkowskiMinPoint + minkowskiSum.dimensions;
            if (slurp::Vec2<float> targetPosition = entityOffsetPosition + positionUpdate;
                math::inRange(targetPosition.x, minkowskiMinPoint.x, minkowskiMaxPoint.x) &&
                math::inRange(targetPosition.y, minkowskiMinPoint.y, minkowskiMaxPoint.y)
            ) {
                if (!collisionInfo.isTrigger && !otherCollisionInfo.isTrigger) {
                    if (math::inRange(entityOffsetPosition.y, minkowskiMinPoint.y, minkowskiMaxPoint.y)) {
                        float xAxisPositionUpdate = positionUpdate.x;
                        if (entityOffsetPosition.x <= otherEntityOffsetPosition.x) {
                            xAxisPositionUpdate = minkowskiMinPoint.x - entityOffsetPosition.x;
                        } else {
                            xAxisPositionUpdate = minkowskiMaxPoint.x - entityOffsetPosition.x;
                        }
                        if (std::abs(xAxisPositionUpdate) < std::abs(positionUpdate.x)) {
                            positionUpdate.x = xAxisPositionUpdate;
                        }
                    }
                    if (math::inRange(entityOffsetPosition.x, minkowskiMinPoint.x, minkowskiMaxPoint.x)) {
                        float yAxisPositionUpdate = positionUpdate.y;
                        if (entityOffsetPosition.y <= otherEntityOffsetPosition.y) {
                            yAxisPositionUpdate = minkowskiMinPoint.y - entityOffsetPosition.y;
                        } else {
                            yAxisPositionUpdate = minkowskiMaxPoint.y - entityOffsetPosition.y;
                        }
                        if (std::abs(yAxisPositionUpdate) < std::abs(positionUpdate.y)) {
                            positionUpdate.y = yAxisPositionUpdate;
                        }
                    }
                }
                collision::handleCollisionEnter(entity, otherEntity);
            } else {
                collision::handleCollisionExit(entity, otherEntity);
            }
        }
        physicsInfo.position += positionUpdate;
    }
}
