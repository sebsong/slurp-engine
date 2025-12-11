#include "Collision.h"
#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo()
        : collisionEnabled(false),
          isStatic(false),
          isTrigger(false),
          shape({}),
          collidingWith(types::set_arena<entity::Entity*>()) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        bool isTrigger,
        const geometry::Shape& shape,
        bool isCentered
    ): CollisionInfo(
        isStatic,
        isTrigger,
        CollisionShape{
            shape,
            isCentered ? -shape.dimensions / 2 : slurp::Vec2<float>::Zero
        }
    ) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        bool isTrigger,
        const CollisionShape& shape
    ): collisionEnabled(true),
       isStatic(isStatic),
       isTrigger(isTrigger),
       shape(shape),
       collidingWith(types::set_arena<entity::Entity*>()) {}

    void handleCollisionEnter(entity::Entity* entity, entity::Entity* otherEntity) {
        if (!entity->collisionInfo.collidingWith.contains(otherEntity)) {
            entity->onCollisionEnter(
                CollisionDetails{
                    otherEntity
                }
            );
        }
        if (!otherEntity->collisionInfo.collidingWith.contains(entity)) {
            otherEntity->onCollisionEnter(
                CollisionDetails{
                    entity
                }
            );
        }
        entity->collisionInfo.collidingWith.insert(otherEntity);
        otherEntity->collisionInfo.collidingWith.insert(entity);
    }

    void handleCollisionExit(entity::Entity* entity, entity::Entity* otherEntity) {
        if (entity->collisionInfo.collidingWith.contains(otherEntity)) {
            entity->onCollisionExit(
                CollisionDetails{
                    otherEntity
                }
            );
        }
        if (otherEntity->collisionInfo.collidingWith.contains(entity)) {
            otherEntity->onCollisionExit(
                CollisionDetails{
                    entity
                }
            );
        }
        entity->collisionInfo.collidingWith.erase(otherEntity);
        otherEntity->collisionInfo.collidingWith.erase(entity);
    }
}
