#include "Entity.h"

#include "EntityPipeline.h"

namespace entity {
    Entity::Entity() noexcept: Entity(
        INVALID_ENTITY_ID,
        "",
        false,
        false,
        {},
        {},
        {},
        false
    ) {}

    Entity::Entity(const Entity& other) noexcept
        : Entity(
            other.id,
            other.name.data(),
            other.enabled,
            other.initialized,
            other.renderInfo,
            other.physicsInfo,
            other.collisionInfo,
            other.shouldDestroy
        ) {}


    Entity::Entity(Entity&& other) noexcept
        : Entity(
            std::move(other.id),
            std::move(other.name),
            std::move(other.enabled),
            std::move(other.initialized),
            std::move(other.renderInfo),
            std::move(other.physicsInfo),
            std::move(other.collisionInfo),
            std::move(other.shouldDestroy)
        ) {}

    Entity::Entity(std::string&& name)
        : Entity(
            INVALID_ENTITY_ID,
            std::move(name),
            true,
            false,
            {},
            {},
            {},
            false
        ) {}

    Entity::Entity(
        std::string&& name,
        const render::RenderInfo& renderInfo,
        const physics::PhysicsInfo& physicsInfo,
        const collision::CollisionInfo& collisionInfo
    ): Entity(
        INVALID_ENTITY_ID,
        std::move(name),
        true,
        false,
        renderInfo,
        physicsInfo,
        collisionInfo,
        false
    ) {}

    Entity::Entity(
        uint32_t id,
        std::string&& name,
        bool enabled,
        bool initialized,
        const render::RenderInfo& renderInfo,
        const physics::PhysicsInfo& physicsInfo,
        const collision::CollisionInfo& collisionInfo,
        bool shouldDestroy
    ): id(id),
       name(std::move(name)),
       enabled(enabled),
       initialized(initialized),
       renderInfo(renderInfo),
       physicsInfo(physicsInfo),
       collisionInfo(collisionInfo),
       shouldDestroy(shouldDestroy) {}

    void Entity::enable() {
        enabled = true;
        if (!initialized) {
            initialize();
        }
    }

    void Entity::initialize() {
        initialized = true;
    }

    void Entity::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {}

    void Entity::handleGamepadInput(uint8_t gamepadIndex, const slurp::GamepadState& gamepadState) {}

    void Entity::updatePhysics(float dt) {
        physicsInfo.updatePhysics(dt);
    }

    void Entity::update(float dt) {}

    void Entity::onCollisionEnter(const collision::CollisionDetails& collisionDetails) {}

    void Entity::onCollisionExit(const collision::CollisionDetails& collisionDetails) {}

    bool Entity::mouseHitTest(const slurp::Vec2<float>& location) const {
        return collisionInfo.shape.hitTest(location - physicsInfo.position);
    }

    void Entity::setTexture(const asset::Sprite* sprite) {
        setTexture(0, sprite);
    }

    void Entity::setTexture(uint8_t spriteIndex, const asset::Sprite* sprite) {
        ASSERT_LOG(renderInfo.numSprites > spriteIndex, "Sprite index out of range");
        renderInfo.sprites[spriteIndex].material.textureId = sprite->material.textureId;
    }

    void Entity::setAlpha(float alpha) {
        for (uint8_t i = 0; i < renderInfo.numSprites; i++) {
            setAlpha(i, alpha);
        }
    }

    void Entity::setAlpha(uint8_t spriteIndex, float alpha) {
        ASSERT_LOG(renderInfo.numSprites > spriteIndex, "Sprite index out of range");
        renderInfo.sprites[spriteIndex].material.alpha = alpha;
    }

    void Entity::applyAlpha(float alpha) {
        for (uint8_t i = 0; i < renderInfo.numSprites; i++) {
            applyAlpha(i, alpha);
        }
    }

    void Entity::applyAlpha(uint8_t spriteIndex, float alpha) {
        ASSERT_LOG(renderInfo.numSprites > spriteIndex, "Sprite index out of range");
        renderInfo.sprites[spriteIndex].material.alpha *= alpha;
    }

    void Entity::playAnimation(
        const asset::SpriteAnimation* animation,
        float totalDuration,
        bool shouldLoop,
        bool playReversed
    ) {
        playAnimation(0, animation, totalDuration, shouldLoop, playReversed);
    }

    void Entity::playAnimation(
        uint8_t spriteIndex,
        const asset::SpriteAnimation* animation,
        float totalDuration,
        bool shouldLoop,
        bool playReversed
    ) {
        ASSERT_LOG(renderInfo.numSprites > spriteIndex, "Sprite index out of range");
        new(&renderInfo.sprites[spriteIndex].animation) asset::SpriteAnimation(*animation);
        renderInfo.sprites[spriteIndex].animation.play(totalDuration, shouldLoop, playReversed);
    }

    void Entity::stopAnimation() {
        stopAnimation(0);
    }

    void Entity::stopAnimation(uint8_t spriteIndex) {
        renderInfo.sprites[spriteIndex].animation.stop();
    }

    Entity& Entity::operator=(const Entity& other) {
        if (this != &other) {
            // NOTE: don't update the id to maintain entity pipeline ordering
            name = other.name;
            enabled = other.enabled;
            renderInfo = other.renderInfo;
            physicsInfo = other.physicsInfo;
            collisionInfo = other.collisionInfo;
            shouldDestroy = other.shouldDestroy;
        }
        return *this;
    }

    Entity& Entity::operator=(const Entity&& other) {
        if (this != &other) {
            // NOTE: don't update the id to maintain entity pipeline ordering
            name = std::move(other.name);
            enabled = std::move(other.enabled);
            renderInfo = std::move(other.renderInfo);
            physicsInfo = std::move(other.physicsInfo);
            collisionInfo = std::move(other.collisionInfo);
            shouldDestroy = std::move(other.shouldDestroy);
        }
        return *this;
    }
}
