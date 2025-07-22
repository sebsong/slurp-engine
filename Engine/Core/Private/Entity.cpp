#include "Entity.h"

namespace slurp {
    Entity::Entity(Entity&& other) noexcept
        : id(std::move(other.id)),
          name(std::move(other.name)),
          enabled(std::move(other.enabled)),
          renderInfo(std::move(other.renderInfo)),
          physicsInfo(std::move(other.physicsInfo)),
          collisionInfo(std::move(other.collisionInfo)),
          shouldDestroy(std::move(other.shouldDestroy)) {}

    Entity::Entity(
        std::string&& name,
        const render::RenderInfo& renderInfo,
        const physics::PhysicsInfo& physicsInfo,
        const collision::CollisionInfo& collisionInfo
    ): id(-1),
       name(std::move(name)),
       enabled(true),
       renderInfo(renderInfo),
       physicsInfo(physicsInfo),
       collisionInfo(collisionInfo),
       shouldDestroy(false) {}
}
