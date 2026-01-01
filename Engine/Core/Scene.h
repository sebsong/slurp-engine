#pragma once
#include "MemoryConstructs.h"

namespace scene {
    struct Scene {
        bool isPaused;
        bool shouldLoad;
        bool shouldUnload;
        memory::MemoryArena sceneMemory;
    };

    /** Game **/
    Scene* create(const std::string& name);

    void start(Scene* scene);

    void stop(Scene* scene);

    void pause(Scene* scene);

    void resume(Scene* scene);

    void transition(Scene* currentScene, Scene* newScene);

    /** Engine **/
    void updateAll();
}
