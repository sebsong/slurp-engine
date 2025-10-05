#include "OpenGL.h"

#include "WinGlad.c"
#include "GLFW/glfw3.h"

#include "Logging.h"
#include "Matrix.h"
#include "RenderApi.h"

namespace open_gl {
    // e.g. [0, 0]                          -> [-1, -1]
    // e.g. [WORLD_WIDTH, WORLD_HEIGHT] -> [1, 1]
    static const slurp::Mat32<float> WorldToOpenGLClipSpaceMatrix = {
        {2.f / CAMERA_WORLD_WIDTH, 0.f},
        {0.f, 2.f / CAMERA_WORLD_HEIGHT},
        {-1.f, -1.f}
    };

    OpenGLRenderWindow::OpenGLRenderWindow(int width, int height, const char* title)
        : _isValid(true),
          _window(nullptr) {
        if (!init(width, height, title)) {
            this->_isValid = false;
        }
    }

    slurp::Vec2<int> OpenGLRenderWindow::getDimensions() const {
        int width, height;
        glfwGetWindowSize(_window, &width, &height);
        return {width, height};
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

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        return true;
    }

    bool OpenGLRenderWindow::isValid() const {
        return _isValid;
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

    RENDER_CREATE_TEXTURE(createTexture) {
        uint32_t textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // TODO: allow specification of texture modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            bitmap.dimensions.width,
            bitmap.dimensions.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bitmap.map
        );
        // glGenerateMipmap(GL_TEXTURE_2D);

        return textureId;
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

        render::object_id shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        if (!validateShaderProgram(shaderProgramId)) { return render::INVALID_ID; }

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return shaderProgramId;
    }

    RENDER_GEN_ARRAY_BUFFER(genArrayBuffer) {
        render::object_id vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        uint32_t vertexBufferId;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glVertexAttribPointer(
            render::POSITION_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec3<float>::DimensionCount,
            GL_FLOAT,
            GL_FALSE,
            sizeof(render::Vertex),
            nullptr
        );
        glEnableVertexAttribArray(render::POSITION_VERTEX_ATTRIBUTE_IDX);

        // TODO: make this optional?
        glVertexAttribPointer(
            render::TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec2<float>::DimensionCount,
            GL_FLOAT,
            GL_FALSE,
            sizeof(render::Vertex),
            reinterpret_cast<void*>(sizeof(render::Vertex::position))
        );
        glEnableVertexAttribArray(render::TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX);

        for (int i = 0; i < vertexCount; i++) {
            render::Vertex& vertex = vertexArray[i];
            // TODO: replace with world to clip space matrix transformation
            vertex.position.x /= CAMERA_WORLD_WIDTH;
            vertex.position.y /= CAMERA_WORLD_HEIGHT;
            // vertex.position *= WorldToOpenGLClipSpaceMatrix;
        }
        // TODO: allow usage control
        glBufferData(GL_ARRAY_BUFFER, sizeof(render::Vertex) * vertexCount, vertexArray, GL_DYNAMIC_DRAW);
        return vertexArrayId;
    }

    RENDER_GEN_ELEMENT_ARRAY_BUFFER(genElementArrayBuffer) {
        render::object_id vertexArrayId = open_gl::genArrayBuffer(vertexArray, vertexCount);

        uint32_t elementBufferId;
        glGenBuffers(1, &elementBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
        // TODO: allow usage control
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elementCount, elementArray, GL_STATIC_DRAW);

        return vertexArrayId;
    }

    static void prepareDraw(
        render::object_id vertexArrayId,
        render::object_id textureId,
        render::object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform
    ) {
        glBindVertexArray(vertexArrayId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUseProgram(shaderProgramId);
        int timeUniformLoc = glGetUniformLocation(shaderProgramId, render::TIME_UNIFORM_NAME);
        if (timeUniformLoc != render::INVALID_ID) {
            glUniform1f(timeUniformLoc, static_cast<GLfloat>(glfwGetTime()));
        }
        int positionTransformUniformLoc = glGetUniformLocation(
            shaderProgramId,
            render::POSITION_TRANSFORM_UNIFORM_NAME
        );
        if (positionTransformUniformLoc != render::INVALID_ID) {
            const slurp::Vec2<float>& position = positionTransform * WorldToOpenGLClipSpaceMatrix;
            glUniform2fv(positionTransformUniformLoc, 1, position.values);
        }
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    RENDER_DRAW_ARRAY(drawArray) {
        prepareDraw(vertexArrayId, textureId, shaderProgramId, positionTransform);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    RENDER_DRAW_ELEMENT_ARRAY(drawElementArray) {
        prepareDraw(vertexArrayId, textureId, shaderProgramId, positionTransform);
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr);

        // glBindVertexArray(render::UNUSED_ID);
        // glUseProgram(render::UNUSED_ID);
    }
}
