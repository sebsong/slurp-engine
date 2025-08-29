#include "OpenGL.h"

#include "WinGlad.c"
#include "GLFW/glfw3.h"

#include "Logging.h"
#include "RenderApi.h"

namespace open_gl {
    OpenGLRenderWindow::OpenGLRenderWindow(int width, int height, const char* title)
        : _isValid(true),
          _window(nullptr),
          _vertexArrayObjectId(render::INVALID_ID),
          _vertexBufferObjectId(render::INVALID_ID),
          _elementBufferObjectId(render::INVALID_ID),
          _shaderProgramId(render::INVALID_ID) {
        if (!init(width, height, title)) {
            this->_isValid = false;
        }
    }

    bool OpenGLRenderWindow::isValid() const {
        return _isValid;
    }

    static void resizeViewport(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    bool OpenGLRenderWindow::init(int width, int height, const char* title) {
        /** Window **/
        if (glfwInit() == GLFW_FALSE) {
            logging::error("Failed to initialize GLFW");
            glfwTerminate();
            return false;
        }
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

    void OpenGLRenderWindow::flip() const {
        glfwSwapBuffers(_window);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    bool OpenGLRenderWindow::shouldTerminate() const {
        return glfwWindowShouldClose(_window);
    }

    void OpenGLRenderWindow::terminate() {
        glfwTerminate();
    }

    RENDER_SET_BACKGROUND_COLOR(setBackgroundColor) {
        glClearColor(red, green, blue, 1);
    }

    RENDER_GEN_ARRAY_BUFFER(genArrayBuffer) {
        render::vertexArrayId vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        uint32_t vertexBufferId;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glVertexAttribPointer(
            render::LOCATION_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec3<float>::Count,
            GL_FLOAT,
            GL_FALSE,
            sizeof(slurp::Vec3<float>),
            nullptr
        );
        glEnableVertexAttribArray(render::LOCATION_VERTEX_ATTRIBUTE_IDX);

        // TODO: allow usage control
        glBufferData(GL_ARRAY_BUFFER, sizeof(slurp::Vec3<float>) * vertexCount, vertexArray, GL_STATIC_DRAW);

        // glBindVertexArray(render::UNUSED_ID);
        // glBindBuffer(GL_ARRAY_BUFFER, render::UNUSED_ID);
        return vertexArrayId;
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

    RENDER_CREATE_SHADER_PROGRAM(createShaderProgram) {
        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShaderId);
        if (!validateShader(vertexShaderId)) { return render::INVALID_ID; }

        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShaderId);
        if (!validateShader(fragmentShaderId)) { return render::INVALID_ID; }

        render::shader_program_id shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        if (!validateShaderProgram(shaderProgramId)) { return render::INVALID_ID; }

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return shaderProgramId;
    }

    RENDER_DRAW_ARRAY(drawArray) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        int timeUniformLoc = glGetUniformLocation(shaderProgramId, render::TIME_UNIFORM_NAME);
        if (timeUniformLoc != render::INVALID_ID) {
            glUniform1f(timeUniformLoc, static_cast<GLfloat>(glfwGetTime()));
        }
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        // glBindVertexArray(render::UNUSED_ID);
        // glUseProgram(render::UNUSED_ID);
    }
}
