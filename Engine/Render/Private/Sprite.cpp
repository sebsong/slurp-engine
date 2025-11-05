#include "Sprite.h"

namespace asset {
    // NOTE: represents a rectangle made of 2 triangles
    static constexpr int SpriteMeshVertexCount = 4;
    static constexpr int SpriteMeshElementCount = 6;
    static constexpr uint32_t SpriteElements[SpriteMeshElementCount] = {0, 1, 2, 2, 3, 0};

    void SpriteAnimation::start() {
        stop();
        isPlaying = true;
    }

    void SpriteAnimation::update(float dt) {
        if (currentFrameDuration >= frameDuration) {
            currentFrameDuration = 0;
            currentFrameIndex++;
            if (currentFrameIndex >= numFrames) {
                if (shouldLoop) {
                    currentFrameIndex = 0;
                } else {
                    stop();
                }
            }
        }

        currentFrameDuration += dt;
    }

    void SpriteAnimation::stop() {
        isPlaying = false;
        currentFrameIndex = 0;
        currentFrameDuration = 0;
    }

    void Sprite::bindShaderUniform(const char* uniformName, float value) const {
        slurp::Globals->RenderApi->bindShaderUniformFloat(material.shaderProgramId, uniformName, value);
    }

    void Sprite::bindShaderUniform(const char* uniformName, bool value) const {
        slurp::Globals->RenderApi->bindShaderUniformBool(material.shaderProgramId, uniformName, value);
    }

    void loadSpriteData(
        Sprite* sprite,
        const Bitmap* bitmap,
        const std::string& vertexShaderSource,
        const std::string& fragmentShaderSource
    ) {
        // TODO: specify scale factor on entity that also applies to collision shapes
        float scale = 1.f;
        slurp::Vec2<float> dimensions = bitmap->dimensions * scale;
        render::Vertex triangleVertices[SpriteMeshVertexCount] = {
            render::Vertex{
                {dimensions.width, dimensions.height},
                {1, 1}
            },
            render::Vertex{
                {0, dimensions.height},
                {0, 1}
            },
            render::Vertex{
                {0, 0},
                {0, 0}
            },
            render::Vertex{
                {dimensions.width, 0},
                {1, 0}
            },
        };

        render::object_id vertexArrayId = slurp::Globals->RenderApi->genElementArrayBuffer(
            triangleVertices,
            SpriteMeshVertexCount,
            SpriteElements,
            SpriteMeshElementCount
        );
        render::object_id textureId = slurp::Globals->RenderApi->createTexture(bitmap);
        render::object_id shaderProgramId = slurp::Globals->RenderApi->createShaderProgram(
            vertexShaderSource.c_str(),
            fragmentShaderSource.c_str()
        );

        sprite->dimensions = dimensions;
        sprite->mesh = Mesh{vertexArrayId, SpriteMeshElementCount};
        sprite->material = Material{textureId, shaderProgramId};
        sprite->isLoaded = true;
    }
}
