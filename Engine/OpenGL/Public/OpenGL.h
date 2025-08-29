#pragma once
#include "RenderApi.h"

struct GLFWwindow;

namespace open_gl {
    class OpenGLRenderWindow {
    public:
        OpenGLRenderWindow(int width, int height, const char* title);

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
        render::vertex_array_id vertexArrayObjectId;
        uint32_t vertexCount;
        render::texture_id textureId;
        render::shader_program_id shaderProgramId;
    };

    RENDER_SET_BACKGROUND_COLOR(setBackgroundColor);

    RENDER_CREATE_TEXTURE(createTexture);

    RENDER_CREATE_SHADER_PROGRAM(createShaderProgram);

    RENDER_GEN_ARRAY_BUFFER(genArrayBuffer);

    RENDER_DRAW_ARRAY(drawArray);
}
