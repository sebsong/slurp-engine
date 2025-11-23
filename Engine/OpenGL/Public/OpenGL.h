#pragma once
#include "RenderApi.h"

#if PLATFORM_WINDOWS
#include <windef.h>
#endif


struct SDL_Window;

namespace open_gl {

    class OpenGLRenderWindow {
    public:
        OpenGLRenderWindow(int width, int height, const char* title, bool isFullscreen);

#if PLATFORM_WINDOWS
        HWND getWin32Handle() const;
#endif

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
        render::object_id vertexArrayObjectId;
        uint32_t elementCount;
        render::object_id textureId;
        render::object_id shaderProgramId;
    };
}
