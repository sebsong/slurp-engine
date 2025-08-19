#pragma once

struct GLFWwindow;
namespace open_gl_slurp {
    class OpenGLRenderWindow {
    public:
        OpenGLRenderWindow(int width, int height, const char* title);

        bool isValid() const;

        void flip() const;

        bool shouldTerminate();

        static void terminate();

    private:
        bool init(int width, int height, const char* title);
        GLFWwindow* _window;
    };
}
