#include "Scene.h"

namespace scene {
    Scene* create(std::string&& name, size_t sizeBytes) {
        Scene* scene = memory::Scene->allocate<Scene>();;
        scene->isPaused = false;
        scene->shouldLoad = false;
        scene->shouldUnload = false;
        scene->sceneMemory = memory::Scene->allocateSubArena(std::move(name), sizeBytes);
        return scene;
    }

    void start(Scene* scene) {
        scene->shouldLoad = true;
    }

    void pause(Scene* scene) {
        scene->isPaused = true;
    }

    void resume(Scene* scene) {
        scene->isPaused = false;
    }

    void transition(Scene* currentScene, Scene* newScene) {
        currentScene->shouldUnload = true;
        start(newScene);
    }

    void load(Scene* scene) {
        // TODO: load assets here?
    }

    void unload(Scene* scene) {
        scene->sceneMemory.freeAll();
    }
}
