#include "Collision.h"

#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo()
        : collisionEnabled(false),
          isStatic(false),
          shape({}),
          collidingWith(std::set<slurp::Entity*>()) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        const geometry::Shape& shape,
        bool isCentered
    ): CollisionInfo(
        isStatic,
        CollisionShape{
            shape,
            isCentered ? -shape.dimensions / 2 : slurp::Vector2<int>::Zero
        }
    ) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        const CollisionShape& shape
    ): collisionEnabled(true),
       isStatic(isStatic),
       shape(shape),
       collidingWith(std::set<slurp::Entity*>()) {}
}
