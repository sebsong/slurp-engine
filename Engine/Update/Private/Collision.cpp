#include "Collision.h"

#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo()
        : collisionEnabled(false),
          isStatic(false),
          shape({}),
          onCollisionEnter(nullptr),
          onCollisionExit(nullptr),
          collidingWith(std::set<slurp::Entity*>()) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        const geometry::Shape& shape,
        bool isCentered
    ): CollisionInfo(
        isStatic,
        shape,
        isCentered,
        nullptr,
        nullptr
    ) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        const geometry::Shape& shape,
        bool isCentered,
        const std::function<void(const slurp::Entity*)>&& onCollisionEnter,
        const std::function<void(const slurp::Entity*)>&& onCollisionExit
    ): CollisionInfo(
        isStatic,
        CollisionShape{
            shape,
            isCentered ? -shape.dimensions / 2 : slurp::Vector2<int>::Zero
        },
        std::move(onCollisionEnter),
        std::move(onCollisionExit)
    ) {}

    CollisionInfo::CollisionInfo(
        bool isStatic,
        const CollisionShape& shape,
        const std::function<void(const slurp::Entity*)>&& onCollisionEnter,
        const std::function<void(const slurp::Entity*)>&& onCollisionExit
    ): collisionEnabled(true),
       isStatic(isStatic),
       shape(shape),
       onCollisionEnter(std::move(onCollisionEnter)),
       onCollisionExit(std::move(onCollisionExit)),
       collidingWith(std::set<slurp::Entity*>()) {}
}
