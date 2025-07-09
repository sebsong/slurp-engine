#include "Collision.h"

#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo(): collisionEnabled(false),
                                    isStatic(false),
                                    shape({}),
                                    onCollisionEnter(NO_OP_ON_COLLISION),
                                    onCollisionExit(NO_OP_ON_COLLISION),
                                    collidingWith(std::set<slurp::Entity*>()) {}

    CollisionInfo::CollisionInfo(
        bool collisionEnabled,
        bool isStatic,
        const geometry::Shape& shape,
        bool isCentered,
        const std::function<void(const slurp::Entity*)>& onCollisionEnter,
        const std::function<void(const slurp::Entity*)>& onCollisionExit
    ): CollisionInfo(
        collisionEnabled,
        isStatic,
        CollisionShape{
            shape,
            isCentered ? -shape.dimensions / 2 : slurp::Vector2<int>::Zero
        },
        onCollisionEnter,
        onCollisionExit
    ) {}

    CollisionInfo::CollisionInfo(
        bool collisionEnabled,
        bool isStatic,
        const CollisionShape& shape,
        const std::function<void(const slurp::Entity*)>& onCollisionEnter,
        const std::function<void(const slurp::Entity*)>& onCollisionExit
    ): collisionEnabled(collisionEnabled),
       isStatic(isStatic),
       shape(shape),
       onCollisionEnter(onCollisionEnter),
       onCollisionExit(onCollisionExit),
       collidingWith(std::set<slurp::Entity*>()) {}
}
