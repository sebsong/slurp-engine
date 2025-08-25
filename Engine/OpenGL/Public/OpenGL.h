#pragma once

struct GLFWwindow;

namespace open_gl {
    typedef uint32_t shader_program_id;

    static constexpr uint32_t INVALID_ID = 0;
    static constexpr uint32_t UNUSED_ID = 0;
    static constexpr uint32_t LOCATION_VERTEX_ATTRIBUTE_IDX = 0;
    static constexpr uint32_t COLOR_VERTEX_ATTRIBUTE_IDX = 1;

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

    shader_program_id createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
}
