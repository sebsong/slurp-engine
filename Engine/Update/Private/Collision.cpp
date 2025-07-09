#include "Collision.h"

#include "Entity.h"

namespace collision {
    CollisionInfo::CollisionInfo(): collisionEnabled(false),
                                    isStatic(false),
                                    shape({}),
                                    collidingWith(std::set<slurp::Entity*>()),
                                    onCollisionEnter(NO_OP_ON_COLLISION),
                                    onCollisionExit(NO_OP_ON_COLLISION){}

    CollisionInfo::CollisionInfo(bool collisionEnabled, bool isStatic, CollisionShape shape,
        std::function<void(const slurp::Entity*)> onCollisionEnter,
        std::function<void(const slurp::Entity*)> onCollisionExit) {};
}
