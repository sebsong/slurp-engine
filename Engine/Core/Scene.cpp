#include "Scene.h"

#include "Debug.h"

#define MAX_NUM_SCENES 20

namespace scene {
    static Scene AllScenes[MAX_NUM_SCENES];
    static uint32_t NewSceneIndex = 0;

    Scene* create(std::string&& name, size_t sizeBytes) {
        Scene* scene = &AllScenes[NewSceneIndex++];
        scene->isPaused = false;
        scene->shouldLoad = false;
        scene->shouldUnload = false;
        scene->sceneMemory = memory::Scene->allocateSubArena(std::move(name), sizeBytes);
        return scene;
    }

    void start(Scene* scene) {
        scene->shouldLoad = true;
    }

    void stop(Scene* scene) {
        scene->shouldUnload = true;
    }

    void pause(Scene* scene) {
        scene->isPaused = true;
    }

    void resume(Scene* scene) {
        scene->isPaused = false;
    }

    void transition(Scene* currentScene, Scene* newScene) {
        stop(currentScene);
        start(newScene);
    }

    static void load(Scene& scene) {
        // TODO: load assets here?
    }

    static void unload(Scene& scene) {
        // TODO: unload assets here?
        scene.sceneMemory.freeAll();
    }

    void updateAll() {
        for (Scene& scene: AllScenes) {
            ASSERT_LOG(!(scene.shouldLoad && scene.shouldUnload), "Trying to load and unload at the same time.");
            if (scene.shouldLoad) {
                load(scene);
                scene.shouldLoad = false;
            } else if (scene.shouldUnload) {
                unload(scene);
                scene.shouldUnload = false;
            }
        }
    }
}
