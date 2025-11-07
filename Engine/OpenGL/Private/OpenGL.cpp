#include "OpenGL.h"

#include "WinGlad.c"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Logging.h"
#include "Matrix.h"
#include "RenderApi.h"
#include "Bitmap.h"

namespace open_gl {
    // e.g. [-WORLD_WIDTH / 2, -WORLD_HEIGHT / 2] -> [-1, -1]
    // e.g. [0, 0]                                -> [0, 0]
    // e.g. [WORLD_WIDTH / 2, WORLD_HEIGHT / 2]   -> [1, 1]
    static const slurp::Mat22<float> WorldToOpenGLClipSpaceMatrix = {
        {2.f / CAMERA_WORLD_WIDTH, 0.f},
        {0.f, 2.f / CAMERA_WORLD_HEIGHT}
    };

    OpenGLRenderWindow::OpenGLRenderWindow(int width, int height, const char* title, bool isFullscreen)
        : _isValid(true),
          _window(nullptr) {
        if (!init(width, height, title, isFullscreen)) {
            this->_isValid = false;
        }
    }

    HWND OpenGLRenderWindow::getWin32Handle() const {
        return glfwGetWin32Window(_window);
    }

    slurp::Vec2<int> OpenGLRenderWindow::getDimensions() const {
        int width, height;
        glfwGetWindowSize(_window, &width, &height);
        return {width, height};
    }

    static void resizeViewport(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    bool OpenGLRenderWindow::init(int width, int height, const char* title, bool isFullscreen) {
        /** Window **/
        if (glfwInit() == GLFW_FALSE) {
            logging::error("Failed to initialize GLFW");
            glfwTerminate();
            return false;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window;
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (isFullscreen) {
            window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
        } else {
            window = glfwCreateWindow(mode->width, mode->height, title, nullptr, nullptr);
        }

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

        if (HIDE_CURSOR) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);

        return true;
    }

    bool OpenGLRenderWindow::isValid() const {
        return _isValid;
    }

    void OpenGLRenderWindow::flip() const {
        glfwSwapBuffers(_window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
            bitmap->dimensions.width,
            bitmap->dimensions.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bitmap->map
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
        if (!validateShader(vertexShaderId)) { return render::INVALID_OBJECT_ID; }

        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShaderId);
        if (!validateShader(fragmentShaderId)) { return render::INVALID_OBJECT_ID; }

        render::object_id shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        if (!validateShaderProgram(shaderProgramId)) { return render::INVALID_OBJECT_ID; }

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return shaderProgramId;
    }

    RENDER_BIND_SHADER_UNIFORM_FLOAT(bindShaderUniformFloat) {
        glUseProgram(shaderProgramId);
        int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
        if (uniformLoc != render::INVALID_OBJECT_ID) {
            glUniform1f(uniformLoc, value);
        }
    }

    RENDER_BIND_SHADER_UNIFORM_BOOL(bindShaderUniformBool) {
        glUseProgram(shaderProgramId);
        int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
        if (uniformLoc != render::INVALID_OBJECT_ID) {
            glUniform1i(uniformLoc, value);
        }
    }

    RENDER_GEN_VERTEX_ARRAY_BUFFER(genVertexArrayBuffer) {
        render::object_id vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        // TODO: return this back out for later resource cleanup
        uint32_t vertexBufferId;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glVertexAttribPointer(
            render::POSITION_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec2<float>::DimensionCount,
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
            vertex.position *= WorldToOpenGLClipSpaceMatrix;
        }
        // TODO: allow usage control
        uint32_t vertexSize = sizeof(render::Vertex);
        glBufferData(GL_ARRAY_BUFFER, vertexSize * vertexCount, vertexArray, GL_STATIC_DRAW);
        return vertexArrayId;
    }

    RENDER_GEN_ELEMENT_ARRAY_BUFFER(genElementArrayBuffer) {
        render::object_id vertexArrayId = open_gl::genVertexArrayBuffer(vertexArray, vertexCount);

        // TODO: return this back out for later resource cleanup
        uint32_t elementBufferId;
        glGenBuffers(1, &elementBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
        // TODO: allow usage control
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elementCount, elementArray, GL_STATIC_DRAW);

        return vertexArrayId;
    }

    static void setZOrderUniform(render::object_id shaderProgramId, int zOrder) {
        int zOrderUniformLoc = glGetUniformLocation(
            shaderProgramId,
            render::Z_ORDER_UNIFORM_NAME
        );
        if (zOrderUniformLoc != render::INVALID_OBJECT_ID) {
            glUniform1f(zOrderUniformLoc, static_cast<float>(zOrder) / (Z_ORDER_MAX + 1));
        }
    }

    static void setColorUniform(render::object_id shaderProgramId, const slurp::Vec4<float>& color) {
        int colorUniformLoc = glGetUniformLocation(
            shaderProgramId,
            render::COLOR_UNIFORM_NAME
        );
        if (colorUniformLoc != render::INVALID_OBJECT_ID) {
            glUniform4fv(colorUniformLoc, 1, color.values);
        }
    }

    static void prepareDraw(
        render::object_id vertexArrayId,
        render::object_id textureId,
        render::object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        float alpha,
        int zOrder
    ) {
        glBindVertexArray(vertexArrayId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUseProgram(shaderProgramId);

        int timeUniformLoc = glGetUniformLocation(shaderProgramId, render::TIME_UNIFORM_NAME);
        if (timeUniformLoc != render::INVALID_OBJECT_ID) {
            glUniform1f(timeUniformLoc, static_cast<GLfloat>(glfwGetTime()));
        }

        int positionTransformUniformLoc = glGetUniformLocation(
            shaderProgramId,
            render::POSITION_TRANSFORM_UNIFORM_NAME
        );
        if (positionTransformUniformLoc != render::INVALID_OBJECT_ID) {
            // TODO: is it better to perform this in the shader?
            const slurp::Vec2<float>& position = positionTransform * WorldToOpenGLClipSpaceMatrix;
            glUniform2fv(positionTransformUniformLoc, 1, position.values);
        }

        int alphaUniformLoc = glGetUniformLocation(
            shaderProgramId,
            render::ALPHA_COORD_UNIFORM_NAME
        );
        if (alphaUniformLoc != render::INVALID_OBJECT_ID) {
            glUniform1f(alphaUniformLoc, alpha);
        }

        setZOrderUniform(shaderProgramId, zOrder);
    }

    RENDER_DRAW_VERTEX_ARRAY(drawVertexArray) {
        prepareDraw(vertexArrayId, textureId, shaderProgramId, positionTransform, alpha, zOrder);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    RENDER_DRAW_ELEMENT_ARRAY(drawElementArray) {
        prepareDraw(vertexArrayId, textureId, shaderProgramId, positionTransform, alpha, zOrder);
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr);
    }

    RENDER_DRAW_POINT(drawPoint) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        setColorUniform(shaderProgramId, color);
        setZOrderUniform(shaderProgramId, -Z_ORDER_MAX);
        glPointSize(size);
        glDrawArrays(GL_POINTS, 0, vertexCount);
    }

    RENDER_DRAW_LINE(drawLine) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        setColorUniform(shaderProgramId, color);
        setZOrderUniform(shaderProgramId, -Z_ORDER_MAX);
        glLineWidth(width);
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    RENDER_DELETE_RESOURCES(deleteResources) {
        glDeleteVertexArrays(1, &vertexArrayId);
        glDeleteBuffers(1, &vertexBufferId);
        glDeleteBuffers(1, &elementBufferId);
        glDeleteProgram(shaderProgramId);
        glDeleteTextures(1, &textureId);
    }
}
