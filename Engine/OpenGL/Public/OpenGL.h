#pragma once

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

        void debugTestDraw() const;

        bool _isValid;
        GLFWwindow* _window;
        uint32_t _vertexArrayObjectId;
        uint32_t _vertexBufferObjectId;
        uint32_t _elementBufferObjectId;
        uint32_t _shaderProgramId;

        uint32_t _otherVertexArrayObjectId;
        uint32_t _otherVertexBufferObjectId;
        uint32_t _otherShaderProgramId;
    };
}
