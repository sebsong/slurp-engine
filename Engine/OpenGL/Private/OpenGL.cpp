#include "OpenGL.h"

#include "WinGlad.c"
#include <GLFW/glfw3.h>

namespace open_gl_slurp {
    static constexpr uint32_t INVALID_ID = -1;
    static constexpr uint32_t UNUSED_ID = 0;
    static constexpr uint32_t LOCATION_VERTEX_ATTRIBUTE_IDX = 0;
    static const char* VERTEX_SHADER_SRC = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";
    static const char* FRAGMENT_SHADER_SRC = R"(
        #version 330 core
        out vec4 Color;
        void main() {
            Color = vec4(1.0f, 0.5f, 1.0f, 1.0f);
        }
    )";
    static const slurp::Vec3<float> TRIANGLE_VERTICES[] = {
        slurp::Vec3(0.f, 0.75f, 0.f),
        slurp::Vec3(-0.75f, -0.75f, 0.f),
        slurp::Vec3(0.75f, -0.75f, 0.f),
    };
    static const slurp::Vec3<float> RECTANGLE_VERTICES[] = {
        slurp::Vec3(-0.75f, 0.75f, 0.f),
        slurp::Vec3(0.75f, 0.75f, 0.f),
        slurp::Vec3(-0.75f, -0.75f, 0.f),
        slurp::Vec3(0.75f, -0.75f, 0.f),
    };
    static const uint32_t RECTANGLE_INDICES[] = {0, 1, 2, 1, 2, 3};

    static void resizeViewport(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    OpenGLRenderWindow::OpenGLRenderWindow(int width, int height, const char* title)
        : _isValid(true),
          _window(nullptr),
          _vertexArrayObjectId(INVALID_ID),
          _vertexBufferObjectId(INVALID_ID),
          _elementBufferObjectId(INVALID_ID),
          _shaderProgramId(INVALID_ID) {
        if (!init(width, height, title)) {
            this->_isValid = false;
        };
    }

    bool OpenGLRenderWindow::isValid() const {
        return _isValid;
    }

    void OpenGLRenderWindow::flip() const {
        debugTestDraw();
        glfwSwapBuffers(_window);
        // glfwPollEvents();
    }

    bool OpenGLRenderWindow::shouldTerminate() const {
        return glfwWindowShouldClose(_window);
    }

    void OpenGLRenderWindow::terminate() {
        glfwTerminate();
    }

    static bool validateShader(uint32_t shaderId) {
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
            logging::error("Failed to compile shader: \n" + std::string(infoLog));
        }
        return success;
    }

    static bool validateShaderProgram(uint32_t programId) {
        int success;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(programId, sizeof(infoLog), nullptr, infoLog);
            logging::error("Failed to link shader program: \n" + std::string(infoLog));
        }
        return success;
    }

    bool OpenGLRenderWindow::init(int width, int height, const char* title) {
        /** Window **/
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

        /** Vertex Array/Buffer Object **/
        glGenVertexArrays(1, &this->_vertexArrayObjectId);
        glBindVertexArray(this->_vertexArrayObjectId);

        glGenBuffers(1, &this->_vertexBufferObjectId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObjectId);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(TRIANGLE_VERTICES), TRIANGLE_VERTICES, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(RECTANGLE_VERTICES), RECTANGLE_VERTICES, GL_STATIC_DRAW);

        glGenBuffers(1, &this->_elementBufferObjectId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferObjectId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RECTANGLE_INDICES), RECTANGLE_INDICES, GL_STATIC_DRAW);

        glVertexAttribPointer(
            LOCATION_VERTEX_ATTRIBUTE_IDX,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(slurp::Vec3<float>),
            nullptr
        );
        glEnableVertexAttribArray(LOCATION_VERTEX_ATTRIBUTE_IDX);

        glBindVertexArray(UNUSED_ID);

        /** Shaders **/
        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &VERTEX_SHADER_SRC, nullptr);
        glCompileShader(vertexShaderId);
        if (!validateShader(vertexShaderId)) { return false; }

        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &FRAGMENT_SHADER_SRC, nullptr);
        glCompileShader(fragmentShaderId);
        if (!validateShader(fragmentShaderId)) { return false; }

        this->_shaderProgramId = glCreateProgram();
        glAttachShader(this->_shaderProgramId, vertexShaderId);
        glAttachShader(this->_shaderProgramId, fragmentShaderId);
        glLinkProgram(this->_shaderProgramId);
        if (!validateShaderProgram(this->_shaderProgramId)) { return false; }

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return true;
    }

    void OpenGLRenderWindow::debugTestDraw() const {
        glClearColor(0.3, 0.2, 0.9, .5f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(this->_vertexArrayObjectId);
        glUseProgram(this->_shaderProgramId);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(UNUSED_ID);
    }
}
