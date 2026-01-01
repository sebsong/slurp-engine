#include "Scene.h"

#include "AudioPlayer.h"
#include "Debug.h"
#include "EntityPipeline.h"

#define MAX_NUM_SCENES 20

namespace scene {
    static Scene* AllScenes[MAX_NUM_SCENES];
    static uint32_t NumRegisteredScenes = 0;

    void registerScene(Scene* scene) {
        AllScenes[NumRegisteredScenes++] = scene;
        scene->isPaused = false;
        scene->shouldLoad = false;
        scene->shouldUnload = false;
    }

    void start(Scene* scene) {
        scene->shouldLoad = true;
    }

    void end(Scene* scene) {
        scene->shouldUnload = true;
    }

    void pause(Scene* scene) {
        scene->isPaused = true;
    }

    void resume(Scene* scene) {
        scene->isPaused = false;
    }

    void transition(Scene* currentScene, Scene* newScene) {
        end(currentScene);
        start(newScene);
    }

    static void load(Scene* scene) {
        scene->init();
        scene->isActive = true;
    }

    static void unload(Scene* scene) {
        // TODO: only remove associated entities and playing sounds from engine systems
        entity::clearAll();
        audio::clearAll();
        scene->isActive = false;
    }

    void updateAll() {
        // TODO: don't need 2 pass if we target entities and audio to unload
        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            ASSERT_LOG(!(scene->shouldLoad && scene->shouldUnload), "Trying to load and unload at the same time.");
            if (scene->shouldUnload) {
                unload(scene);
                scene->shouldUnload = false;
            }
        }
        for (uint32_t i = 0; i < NumRegisteredScenes; i++) {
            Scene* scene = AllScenes[i];
            ASSERT_LOG(!(scene->shouldLoad && scene->shouldUnload), "Trying to load and unload at the same time.");
            if (scene->shouldLoad) {
                load(scene);
                scene->shouldLoad = false;
            }
        }
    }
}
