#include "Base.h"

namespace base {
    static const geometry::Shape BaseShape = {geometry::Rect, {32, 10}};
    static const slurp::Vec2<float> RenderOffset = {0, 8};
    static slurp::Vec2<float> SpawnOffset = {0, -5};

    static constexpr uint32_t GoldGoal = 100;
    static const char* ProgressUniformName = "progress";

    Base::Base(): Entity(
        "Base",
        render::RenderInfo(slurp::Globals->GameAssets->baseSprite, true, 0, RenderOffset),
        physics::PhysicsInfo(),
        collision::CollisionInfo(
            true,
            true,
            BaseShape,
            true
        )
    ) {}

    slurp::Vec2<float> Base::getDropOffLocation() const {
        return physicsInfo.position + SpawnOffset;
    }

    void Base::dropOff() {
        gold++;
    }

    void Base::initialize() {
        Entity::initialize();
    }

    void Base::update(float dt) {
        Entity::update(dt);

        game::Assets->storageSiloFill->bindShaderUniform(ProgressUniformName, static_cast<float>(gold) / GoldGoal);

        // debug::drawPoint(getDropOffLocation(), 4, DEBUG_GREEN_COLOR);
    }

    void Base::handleMouseAndKeyboardInput(
        const slurp::MouseState& mouseState,
        const slurp::KeyboardState& keyboardState
    ) {
        Entity::handleMouseAndKeyboardInput(mouseState, keyboardState);

        // TODO: add costs and build times

        if (keyboardState.justPressed(slurp::KeyboardCode::NUM_1)) {
            worker::Worker* newWorker = game::State->workers.newInstance();
            newWorker->physicsInfo.position = physicsInfo.position + SpawnOffset;
        }

        if (keyboardState.justPressed(slurp::KeyboardCode::NUM_2)) {
            game::State->mineSiteSpawner.spawnMineSite();
        }

        if (keyboardState.justPressed(slurp::KeyboardCode::NUM_3)) {
            antibody::Antibody* newAntibody = game::State->antibodies.newInstance();
            newAntibody->physicsInfo.position = physicsInfo.position + SpawnOffset;
        }
    }
}
