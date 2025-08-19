#include "OpenGL.h"

#include "WinGlad.c"
#include <GLFW/glfw3.h>

namespace open_gl_slurp {
    static void resizeViewport(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    OpenGLRenderWindow::OpenGLRenderWindow(int width, int height, const char* title): _window(nullptr) {
        init(width, height, title);
    }

    bool OpenGLRenderWindow::isValid() const {
        return _window != nullptr;
    }

    void OpenGLRenderWindow::flip() const {
        glfwSwapBuffers(_window);
        // glfwPollEvents();
    }

    bool OpenGLRenderWindow::shouldTerminate() {
        return glfwWindowShouldClose(_window);
    }

    void OpenGLRenderWindow::terminate() {
        glfwTerminate();
    }

    bool OpenGLRenderWindow::init(int width, int height, const char* title) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            logging::error("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }
        _window = window;
        glfwMakeContextCurrent(_window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            logging::error("Failed to load glad GL loader");
            return false;
        }
        glfwSetFramebufferSizeCallback(_window, resizeViewport);
        return true;
    }
}
