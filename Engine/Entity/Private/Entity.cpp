#include "Entity.h"

namespace entity {
    Entity::Entity() noexcept: id(INVALID_ENTITY_ID), enabled(false), shouldDestroy(false) {}

    Entity::Entity(const Entity& other) noexcept
        : id(other.id),
          name(other.name),
          enabled(other.enabled),
          renderInfo(other.renderInfo),
          physicsInfo(other.physicsInfo),
          collisionInfo(other.collisionInfo),
          shouldDestroy(other.shouldDestroy) {}

    Entity::Entity(Entity&& other) noexcept
        : id(std::move(other.id)),
          name(std::move(other.name)),
          enabled(std::move(other.enabled)),
          renderInfo(std::move(other.renderInfo)),
          physicsInfo(std::move(other.physicsInfo)),
          collisionInfo(std::move(other.collisionInfo)),
          shouldDestroy(std::move(other.shouldDestroy)) {}

    Entity::Entity(std::string&& name)
        : id(INVALID_ENTITY_ID),
          name(std::move(name)),
          enabled(true),
          shouldDestroy(false) {}

    Entity::Entity(
        std::string&& name,
        const render::RenderInfo& renderInfo,
        const physics::PhysicsInfo& physicsInfo,
        const collision::CollisionInfo& collisionInfo
    ): id(INVALID_ENTITY_ID),
       name(std::move(name)),
       enabled(true),
       renderInfo(renderInfo),
       physicsInfo(physicsInfo),
       collisionInfo(collisionInfo),
       shouldDestroy(false) {}

    void Entity::updatePhysics(float dt) {
        physicsInfo.updatePhysics(dt);
    }

    bool Entity::mouseHitTest(const slurp::Vec2<float>& location) const {
        return collisionInfo.shape.hitTest(location - physicsInfo.position);
    }
}
