#pragma once
#include "RenderApi.h"

struct GLFWwindow;

namespace open_gl {

    class OpenGLRenderWindow {
    public:
        OpenGLRenderWindow(int width, int height, const char* title);

        slurp::Vec2<int> getDimensions() const;

        bool isValid() const;

        void flip() const;

        bool shouldTerminate() const;

        static void terminate();

    private:
        bool init(int width, int height, const char* title);

        bool _isValid;
        GLFWwindow* _window;
    };

    struct OpenGLRenderInfo {
        render::object_id vertexArrayObjectId;
        uint32_t elementCount;
        render::object_id textureId;
        render::object_id shaderProgramId;
    };
}
