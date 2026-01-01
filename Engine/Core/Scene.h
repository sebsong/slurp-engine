#pragma once
#include "Entity.h"

namespace scene {
    struct Scene {
        bool isActive;
        bool isPaused;
        bool shouldLoad;
        bool shouldUnload;

        virtual void init() = 0;
        Scene() = default;
        virtual ~Scene() = default;
    };

    /** Game **/
    void registerScene(Scene* scene);

    void start(Scene* scene);

    void end(Scene* scene);

    void pause(Scene* scene);

    void resume(Scene* scene);

    void transition(Scene* currentScene, Scene* newScene);

    /** Engine **/
    void updateAll();
}
