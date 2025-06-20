#include "Update.h"

#include "Core.h"
#include "Math.h"
#include "Render.h"
#include "Collision.h"

namespace update {
    template<typename EntityCollection> // TODO: constrain this type
    void updatePosition(slurp::Entity& entity, const EntityCollection& allEntities, float dt) {
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

            collision::CollisionSquare minkowskiSum = collision::getMinkowskiSum(entity.collisionSquare, otherEntity.collisionSquare);
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
                return;
            }
        }
        entity.position += targetPositionUpdate;
    }
}
