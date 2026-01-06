#include "OpenGL.h"

#include "Settings.h"
#include "Logging.h"
#include "Matrix.h"
#include "RenderApi.h"
#include "RenderInfo.h"
#include "Bitmap.h"

#include <SDL3/SDL.h>
#include <Glad.c>

namespace render {
    // e.g. [-WORLD_WIDTH / 2, -WORLD_HEIGHT / 2] -> [-1, -1]
    // e.g. [0, 0]                                -> [0, 0]
    // e.g. [WORLD_WIDTH / 2, WORLD_HEIGHT / 2]   -> [1, 1]
    static const slurp::Mat22<float> WorldToOpenGLClipSpaceMatrix = {
        {2.f / WORLD_WIDTH, 0.f},
        {0.f, 2.f / WORLD_HEIGHT}
    };

    void setBackgroundColor(float red, float green, float blue) {
        glClearColor(red, green, blue, 1);
    }

    object_id createTexture(const asset::Bitmap* bitmap) {
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

    object_id createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderId, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShaderId);
        if (!validateShader(vertexShaderId)) { return INVALID_OBJECT_ID; }

        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShaderId);
        if (!validateShader(fragmentShaderId)) { return INVALID_OBJECT_ID; }

        object_id shaderProgramId = glCreateProgram();
        glAttachShader(shaderProgramId, vertexShaderId);
        glAttachShader(shaderProgramId, fragmentShaderId);
        glLinkProgram(shaderProgramId);
        if (!validateShaderProgram(shaderProgramId)) { return INVALID_OBJECT_ID; }

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        return shaderProgramId;
    }

    void bindShaderUniformFloat(object_id shaderProgramId, const char* uniformName, float value) {
        glUseProgram(shaderProgramId);
        int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
        if (uniformLoc != INVALID_OBJECT_ID) {
            glUniform1f(uniformLoc, value);
        }
    }

    void bindShaderUniformBool(object_id shaderProgramId, const char* uniformName, bool value) {
        glUseProgram(shaderProgramId);
        int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
        if (uniformLoc != INVALID_OBJECT_ID) {
            glUniform1i(uniformLoc, value);
        }
    }

    void bindShaderUniformVec4(object_id shaderProgramId, const char* uniformName, slurp::Vec4<float> value) {
        glUseProgram(shaderProgramId);
        int uniformLoc = glGetUniformLocation(shaderProgramId, uniformName);
        if (uniformLoc != INVALID_OBJECT_ID) {
            glUniform4fv(uniformLoc, 1, value.values);
        }
    }

    object_id genVertexArrayBuffer(Vertex vertexArray[], int vertexCount) {
        object_id vertexArrayId;
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        // TODO: return this back out for later resource cleanup
        uint32_t vertexBufferId;
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glVertexAttribPointer(
            POSITION_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec2<float>::DimensionCount,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            nullptr
        );
        glEnableVertexAttribArray(POSITION_VERTEX_ATTRIBUTE_IDX);

        // TODO: make this optional?
        glVertexAttribPointer(
            TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX,
            slurp::Vec2<float>::DimensionCount,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(sizeof(Vertex::position))
        );
        glEnableVertexAttribArray(TEXTURE_COORD_VERTEX_ATTRIBUTE_IDX);

        for (int i = 0; i < vertexCount; i++) {
            Vertex& vertex = vertexArray[i];
            vertex.position *= WorldToOpenGLClipSpaceMatrix;
        }
        // TODO: allow usage control
        uint32_t vertexSize = sizeof(Vertex);
        glBufferData(GL_ARRAY_BUFFER, vertexSize * vertexCount, vertexArray, GL_DYNAMIC_DRAW);
        return vertexArrayId;
    }

    object_id genElementArrayBuffer(
        Vertex vertexArray[],
        int vertexCount,
        const uint32_t elementArray[],
        int elementCount
    ) {
        object_id vertexArrayId = genVertexArrayBuffer(vertexArray, vertexCount);

        // TODO: return this back out for later resource cleanup
        uint32_t elementBufferId;
        glGenBuffers(1, &elementBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
        // TODO: allow usage control
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elementCount, elementArray, GL_DYNAMIC_DRAW);

        return vertexArrayId;
    }

    static void setZOrderUniform(object_id shaderProgramId, int zOrder) {
        int zOrderUniformLoc = glGetUniformLocation(
            shaderProgramId,
            Z_ORDER_UNIFORM_NAME
        );
        if (zOrderUniformLoc != INVALID_OBJECT_ID) {
            glUniform1f(zOrderUniformLoc, static_cast<float>(zOrder) / (Z_ORDER_MAX + 1));
        }
    }

    static void setColorUniform(object_id shaderProgramId, const slurp::Vec4<float>& color) {
        int colorUniformLoc = glGetUniformLocation(
            shaderProgramId,
            COLOR_UNIFORM_NAME
        );
        if (colorUniformLoc != INVALID_OBJECT_ID) {
            glUniform4fv(colorUniformLoc, 1, color.values);
        }
    }

    static void prepareDraw(
        object_id vertexArrayId,
        object_id textureId,
        object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        const slurp::Vec4<float>& srcColor,
        const slurp::Vec4<float>& dstColor,
        const slurp::Vec4<float>& colorOverride,
        float alpha,
        int zOrder
    ) {
        glBindVertexArray(vertexArrayId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUseProgram(shaderProgramId);

        int timeUniformLoc = glGetUniformLocation(shaderProgramId, TIME_UNIFORM_NAME);
        if (timeUniformLoc != INVALID_OBJECT_ID) {
            glUniform1f(timeUniformLoc, SDL_GetTicks() / 1000.f);
        }

        int positionTransformUniformLoc = glGetUniformLocation(
            shaderProgramId,
            POSITION_TRANSFORM_UNIFORM_NAME
        );
        if (positionTransformUniformLoc != INVALID_OBJECT_ID) {
            // TODO: is it better to perform this in the shader?
            const slurp::Vec2<float>& position = positionTransform * WorldToOpenGLClipSpaceMatrix;
            glUniform2fv(positionTransformUniformLoc, 1, position.values);
        }

        int alphaUniformLoc = glGetUniformLocation(
            shaderProgramId,
            ALPHA_UNIFORM_NAME
        );
        if (alphaUniformLoc != INVALID_OBJECT_ID) {
            glUniform1f(alphaUniformLoc, alpha);
        }

        bindShaderUniformVec4(shaderProgramId, SRC_COLOR_UNIFORM_NAME, srcColor);
        bindShaderUniformVec4(shaderProgramId, SRC_COLOR_UNIFORM_NAME, dstColor);

        bindShaderUniformVec4(shaderProgramId, COLOR_OVERRIDE_UNIFORM_NAME, colorOverride);

        bindShaderUniformFloat(shaderProgramId, ALPHA_UNIFORM_NAME, alpha);

        setZOrderUniform(shaderProgramId, zOrder);
    }

    void drawVertexArray(
        object_id vertexArrayId,
        int vertexCount,
        object_id textureId,
        object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        const slurp::Vec4<float>& srcColor,
        const slurp::Vec4<float>& dstColor,
        const slurp::Vec4<float>& colorOverride,
        float alpha,
        int zOrder
    ) {
        prepareDraw(
            vertexArrayId,
            textureId,
            shaderProgramId,
            positionTransform,
            srcColor,
            dstColor,
            colorOverride,
            alpha,
            zOrder
        );
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void drawElementArray(
        object_id vertexArrayId,
        int elementCount,
        object_id textureId,
        object_id shaderProgramId,
        const slurp::Vec2<float>& positionTransform,
        const slurp::Vec4<float>& srcColor,
        const slurp::Vec4<float>& dstColor,
        const slurp::Vec4<float>& colorOverride,
        float alpha,
        int zOrder
    ) {
        prepareDraw(
            vertexArrayId,
            textureId,
            shaderProgramId,
            positionTransform,
            srcColor,
            dstColor,
            colorOverride,
            alpha,
            zOrder
        );
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr);
    }

    void drawPoint(
        object_id vertexArrayId,
        int vertexCount,
        object_id shaderProgramId,
        float size,
        const slurp::Vec4<float>& color
    ) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        setColorUniform(shaderProgramId, color);
        setZOrderUniform(shaderProgramId, -Z_ORDER_MAX - 1);
        glPointSize(size);
        glDrawArrays(GL_POINTS, 0, vertexCount);
    }

    void drawLine(
        object_id vertexArrayId,
        int vertexCount,
        object_id shaderProgramId,
        float width,
        const slurp::Vec4<float>& color
    ) {
        glBindVertexArray(vertexArrayId);
        glUseProgram(shaderProgramId);
        setColorUniform(shaderProgramId, color);
        setZOrderUniform(shaderProgramId, -Z_ORDER_MAX - 1);
        glLineWidth(width);
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void deleteResources(
        object_id vertexArrayId,
        object_id vertexBufferId,
        object_id elementBufferId,
        object_id shaderProgramId,
        object_id textureId
    ) {
        glDeleteVertexArrays(1, &vertexArrayId);
        glDeleteBuffers(1, &vertexBufferId);
        glDeleteBuffers(1, &elementBufferId);
        glDeleteProgram(shaderProgramId);
        glDeleteTextures(1, &textureId);
    }
}
