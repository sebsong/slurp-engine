#pragma once
#include "RenderApi.h"


struct SDL_Window;

namespace render {

    class OpenGLRenderWindow {
    public:
        OpenGLRenderWindow(int width, int height, const char* title, bool isFullscreen);

        slurp::Vec2<int> getDimensions() const;

        bool isValid() const;

        void flip() const;

        bool shouldTerminate() const;

        static void terminate();

    private:
        bool init(int width, int height, const char* title, bool isFullscreen);

        bool _isValid;
        SDL_Window* _window;
    };

    struct OpenGLRenderInfo {
        object_id vertexArrayObjectId;
        uint32_t elementCount;
        object_id textureId;
        object_id shaderProgramId;
    };
}
