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

    static const char* vertexShaderSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";
    static void validateShader(uint32_t shaderId) {
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
            logging::error("Failed to compile shader: \n" + std::string(infoLog));
        }
    }
    static void initShaders() {
        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShaderId);
        validateShader(vertexShaderId);
    }

    static void debugTestDraw() {
        glClearColor(0.3, 0.2, 0.9, .5f);
        glClear(GL_COLOR_BUFFER_BIT);

        uint32_t vertexBufferObjectId;
        glGenBuffers(1, &vertexBufferObjectId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectId);
        float vertices[] = {
            0.f, 0.5f, 0.f,
            -0.5f, -0.5f, 0.f,
            0.5f, -0.5f, 0.f,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        initShaders();
    }


    void OpenGLRenderWindow::flip() const {
        debugTestDraw();
        glfwSwapBuffers(_window);
        // glfwPollEvents();
    }

    bool OpenGLRenderWindow::shouldTerminate() const{
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
