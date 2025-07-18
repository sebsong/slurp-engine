#include "Collision.h"

#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo()
        : collisionEnabled(false),
          isStatic(false),
          isTrigger(false),
          shape({}),
          collidingWith(std::set<slurp::Entity*>()) {}

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
            isCentered ? -shape.dimensions / 2 : slurp::Vector2<int>::Zero
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
       collidingWith(std::set<slurp::Entity*>()) {}
}
