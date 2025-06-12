#include "Update.h"
#include "Core.h"
#include "Render.h"

namespace update {
    static int getAxisPositionUpdate(
        const int currentAxisPosition,
        const int targetAxisPositionUpdate,
        const render::ColorPaletteIdx targetTilemapValue,
        const uint8_t tileSize
    ) {
        if (targetTilemapValue == EMPTY_COLOR_PALETTE_IDX) // TODO: probably have a collision map
        {
            // tile is empty
            return targetAxisPositionUpdate;
        }

        // tile is occupied
        int targetAxisPosition = currentAxisPosition + targetAxisPositionUpdate;
        if (targetAxisPosition > currentAxisPosition) {
            int updatedAxisTilemapPosition = targetAxisPosition / tileSize;
            int updatedAxisPosition = updatedAxisTilemapPosition * tileSize - 1;
            return updatedAxisPosition - currentAxisPosition;
        }
        if (targetAxisPosition < currentAxisPosition) {
            int updatedAxisTilemapPosition = currentAxisPosition / tileSize;
            int updatedAxisPosition = updatedAxisTilemapPosition * tileSize;
            return updatedAxisPosition - currentAxisPosition;
        }

        return 0;
    }

    static slurp::Vector2<int> getPositionUpdate(
        const slurp::Vector2<int>& currentPosition,
        const slurp::Vector2<int>& targetPositionUpdate,
        const slurp::Tilemap& tilemap
    ) {
        const slurp::Vector2<int> currentTilemapPosition = currentPosition / tilemap.tileSize;
        const slurp::Vector2<int> targetPosition = currentPosition + targetPositionUpdate;
        const slurp::Vector2<int> targetTilemapPosition = targetPosition / tilemap.tileSize;
        if (
            targetTilemapPosition.x >= 0 && targetTilemapPosition.x < TILEMAP_WIDTH &&
            targetTilemapPosition.y >= 0 && targetTilemapPosition.y < TILEMAP_HEIGHT
        ) {
            render::ColorPaletteIdx targetXAxisTilemapValue = tilemap.map[currentTilemapPosition.y][
                targetTilemapPosition.x];
            int xAxisPositionUpdate = getAxisPositionUpdate(
                currentPosition.x,
                targetPositionUpdate.x,
                targetXAxisTilemapValue,
                tilemap.tileSize
            );

            render::ColorPaletteIdx targetYAxisTilemapValue = tilemap.map[targetTilemapPosition.y][
                currentTilemapPosition.x];
            int yAxisPositionUpdate = getAxisPositionUpdate(
                currentPosition.y,
                targetPositionUpdate.y,
                targetYAxisTilemapValue,
                tilemap.tileSize
            );

            return {xAxisPositionUpdate, yAxisPositionUpdate};
        }
        return slurp::Vector2<int>::Zero;
    }

    void updatePosition(slurp::Entity& entity, const slurp::Tilemap& tilemap, float dt) {
        slurp::Vector2<int> targetPositionUpdate = (entity.direction * entity.speed * dt);
        slurp::Vector2<int> positionUpdate = targetPositionUpdate;
        for (const slurp::Vector2<int>& relativeCollisionPoint: entity.relativeCollisionPoints) {
            slurp::Vector2<int> collisionPoint = entity.position + relativeCollisionPoint;
            slurp::Vector2<int> pointPositionUpdate = getPositionUpdate(
                collisionPoint,
                targetPositionUpdate,
                tilemap
            );
            if (std::abs(pointPositionUpdate.x) < std::abs(positionUpdate.x)) {
                positionUpdate.x = pointPositionUpdate.x;
            }
            if (std::abs(pointPositionUpdate.y) < std::abs(positionUpdate.y)) {
                positionUpdate.y = pointPositionUpdate.y;
            }
        }
        entity.position += positionUpdate;
    }
}
