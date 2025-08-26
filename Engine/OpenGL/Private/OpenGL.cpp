#include "OpenGL.h"

#include "WinGlad.c"
#include "GLFW/glfw3.h"

#include "Logging.h"
#include "RenderApi.h"

namespace open_gl {
    static const char* VERTEX_SHADER_SRC = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;
        out vec3 fragColor;

        void main() {
            gl_Position = vec4(position, 1.0);
            fragColor = color;
        }
    )";
    static const char* FRAGMENT_SHADER_SRC = R"(
        #version 330 core
        in vec3 fragColor;
        out vec4 Color;

        void main() {
            Color = vec4(fragColor, 1.f);
        }
    )";
    static const char* OTHER_VERTEX_SHADER_SRC = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";
    static const char* OTHER_FRAGMENT_SHADER_SRC = R"(
        #version 330 core
        uniform vec2 redBlueColor;
        out vec4 Color;

        void main() {
            Color = vec4(redBlueColor.x, 0.5f, redBlueColor.y, 1.0f);
        }
    )";

    static const slurp::Vec3<float> TRIANGLE_VERTICES[] = {
        // Vertex 0
        slurp::Vec3(-0.5f, 0.75f, 0.f),
        slurp::Vec3(1.f, 0.f, 0.f),

        // Vertex 1
        slurp::Vec3(-1.f, -0.75f, 0.f),
        slurp::Vec3(0.f, 1.f, 0.f),

        // Vertex 2
        slurp::Vec3(0.f, -0.75f, 0.f),
        slurp::Vec3(0.f, 0.f, 1.f),

        // slurp::Vec3(0.f, 0.75f, 0.f),
        // slurp::Vec3(-0.75f, -0.75f, 0.f),
        // slurp::Vec3(0.75f, -0.75f, 0.f),
    };
    static const slurp::Vec3<float> OTHER_TRIANGLE_VERTICES[] = {
        slurp::Vec3(0.5f, 0.75f, 0.f),
        slurp::Vec3(1.f, -0.75f, 0.f),
        slurp::Vec3(0.f, -0.75f, 0.f),
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
          _vertexArrayObjectId(render::INVALID_ID),
          _vertexBufferObjectId(render::INVALID_ID),
          _elementBufferObjectId(render::INVALID_ID),
          _shaderProgramId(render::INVALID_ID) {
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

        /** Vertex Array/Buffer Object **/
        // glGenVertexArrays(1, &this->_vertexArrayObjectId);
        // glBindVertexArray(this->_vertexArrayObjectId);
        //
        // glGenBuffers(1, &this->_vertexBufferObjectId);
        // glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObjectId);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(TRIANGLE_VERTICES), TRIANGLE_VERTICES, GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(RECTANGLE_VERTICES), RECTANGLE_VERTICES, GL_STATIC_DRAW);

        // glGenBuffers(1, &this->_elementBufferObjectId);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferObjectId);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RECTANGLE_INDICES), RECTANGLE_INDICES, GL_STATIC_DRAW);

        // glVertexAttribPointer(
        //     render::LOCATION_VERTEX_ATTRIBUTE_IDX,
        //     3,
        //     GL_FLOAT,
        //     GL_FALSE,
        //     sizeof(slurp::Vec3<float>) * 2,
        //     nullptr
        // );
        // glEnableVertexAttribArray(render::LOCATION_VERTEX_ATTRIBUTE_IDX);
        // glVertexAttribPointer(
        //     render::COLOR_VERTEX_ATTRIBUTE_IDX,
        //     3,
        //     GL_FLOAT,
        //     GL_FALSE,
        //     sizeof(slurp::Vec3<float>) * 2,
        //     reinterpret_cast<void*>(sizeof(slurp::Vec3<float>))
        // );
        // glEnableVertexAttribArray(render::COLOR_VERTEX_ATTRIBUTE_IDX);
        //
        // glBindVertexArray(render::UNUSED_ID);

        // OTHER
        glGenVertexArrays(1, &this->_otherVertexArrayObjectId);
        glBindVertexArray(this->_otherVertexArrayObjectId);

        glGenBuffers(1, &this->_otherVertexBufferObjectId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_otherVertexBufferObjectId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(OTHER_TRIANGLE_VERTICES), OTHER_TRIANGLE_VERTICES, GL_STATIC_DRAW);

        glVertexAttribPointer(
            render::LOCATION_VERTEX_ATTRIBUTE_IDX,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(slurp::Vec3<float>),
            nullptr
        );
        glEnableVertexAttribArray(render::LOCATION_VERTEX_ATTRIBUTE_IDX);

        glBindVertexArray(render::UNUSED_ID);

        /** Shaders **/
        // uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        // glShaderSource(vertexShaderId, 1, &VERTEX_SHADER_SRC, nullptr);
        // glCompileShader(vertexShaderId);
        // if (!validateShader(vertexShaderId)) { return false; }
        //
        // uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        // glShaderSource(fragmentShaderId, 1, &FRAGMENT_SHADER_SRC, nullptr);
        // glCompileShader(fragmentShaderId);
        // if (!validateShader(fragmentShaderId)) { return false; }
        //
        // this->_shaderProgramId = glCreateProgram();
        // glAttachShader(this->_shaderProgramId, vertexShaderId);
        // glAttachShader(this->_shaderProgramId, fragmentShaderId);
        // glLinkProgram(this->_shaderProgramId);
        // if (!validateShaderProgram(this->_shaderProgramId)) { return false; }
        //
        // glDeleteShader(vertexShaderId);
        // glDeleteShader(fragmentShaderId);

        // OTHER
        uint32_t otherVertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(otherVertexShaderId, 1, &OTHER_VERTEX_SHADER_SRC, nullptr);
        glCompileShader(otherVertexShaderId);
        if (!validateShader(otherVertexShaderId)) { return false; }

        uint32_t otherFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(otherFragmentShaderId, 1, &OTHER_FRAGMENT_SHADER_SRC, nullptr);
        glCompileShader(otherFragmentShaderId);
        if (!validateShader(otherFragmentShaderId)) { return false; }

        this->_otherShaderProgramId = glCreateProgram();
        glAttachShader(this->_otherShaderProgramId, otherVertexShaderId);
        glAttachShader(this->_otherShaderProgramId, otherFragmentShaderId);
        glLinkProgram(this->_otherShaderProgramId);
        if (!validateShaderProgram(this->_otherShaderProgramId)) { return false; }

        glDeleteShader(otherVertexShaderId);
        glDeleteShader(otherFragmentShaderId);

        return true;
    }

    static void drawArrays(uint32_t vertexArrayObjectId, uint32_t shaderProgramId, int vertexCount) {
        glBindVertexArray(vertexArrayObjectId);
        glUseProgram(shaderProgramId);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(render::UNUSED_ID);
        glUseProgram(render::UNUSED_ID);
    }

    void OpenGLRenderWindow::debugTestDraw() const {
        // glClearColor(0.3, 0.2, 0.9, .5f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // drawArrays(this->_vertexArrayObjectId, this->_shaderProgramId, 3);

        // OTHER
        float time = glfwGetTime();
        float blueValue = 0.5f * (1.f + std::sin(time));
        float redValue = 0.5f * (1.f + std::cos(time));
        int fragmentColorLocation = glGetUniformLocation(this->_otherShaderProgramId, "redBlueColor");
        glUseProgram(this->_otherShaderProgramId);
        glUniform2f(fragmentColorLocation, redValue, blueValue);
        drawArrays(this->_otherVertexArrayObjectId, this->_otherShaderProgramId, 3);
    }

    GEN_ARRAY_BUFFER(genArrayBuffer) {
        render::vertexArrayId vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        uint32_t vertexBufferId;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        // TODO: allow usage control
        glBufferData(GL_ARRAY_BUFFER, sizeof(slurp::Vec3<float>) * vertexCount, TRIANGLE_VERTICES, GL_STATIC_DRAW);

        // glBindVertexArray(render::UNUSED_ID);
        // glBindBuffer(GL_ARRAY_BUFFER, render::UNUSED_ID);
        return vertexArrayId;
    }

    CREATE_SHADER_PROGRAM(createShaderProgram) {
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

    DRAW_ARRAY(drawArray) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // glBindVertexArray(render::UNUSED_ID);
        // glUseProgram(render::UNUSED_ID);
    }
}
