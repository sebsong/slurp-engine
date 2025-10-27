#include "Debug.h"

#include "Global.h"
#include "RenderApi.h"
#if DEBUG
static constexpr uint32_t PointVertexCount = 1;
static constexpr uint32_t LineVertexCount = 2;

namespace debug {
    void drawPoint(
        const slurp::Vec2<float>& point,
        float size,
        const slurp::Vec4<float>& color
    ) {
        render::Vertex vertexArray[PointVertexCount] = {
            {point, {}},
        };
        render::object_id vertexArrayId = slurp::Globals->RenderApi->
                genVertexArrayBuffer(vertexArray, PointVertexCount);
        asset::ShaderSource* vertexShaderSource = asset::loadVertexShaderSource("basic.glsl");
        asset::ShaderSource* fragmentShaderSource = asset::loadFragmentShaderSource("basic.glsl");
        render::object_id shaderProgramId = slurp::Globals->RenderApi->createShaderProgram(
            vertexShaderSource->source.c_str(),
            fragmentShaderSource->source.c_str()
        );
        slurp::Globals->RenderApi->drawPoint(
            vertexArrayId,
            PointVertexCount,
            shaderProgramId,
            size,
            color
        );
        slurp::Globals->RenderApi->deleteResources(
            vertexArrayId,
            render::INVALID_OBJECT_ID,
            render::INVALID_OBJECT_ID,
            shaderProgramId,
            render::INVALID_OBJECT_ID
        );
    }

    void drawLine(
        const slurp::Vec2<float>& start,
        const slurp::Vec2<float>& end,
        float lineWidth,
        const slurp::Vec4<float>& color
    ) {
        render::Vertex vertexArray[LineVertexCount] = {
            {start, {}},
            {end, {}}
        };
        render::object_id vertexArrayId = slurp::Globals->RenderApi->genVertexArrayBuffer(vertexArray, LineVertexCount);
        asset::ShaderSource* vertexShaderSource = asset::loadVertexShaderSource("basic.glsl");
        asset::ShaderSource* fragmentShaderSource = asset::loadFragmentShaderSource("basic.glsl");
        render::object_id shaderProgramId = slurp::Globals->RenderApi->createShaderProgram(
            vertexShaderSource->source.c_str(),
            fragmentShaderSource->source.c_str()
        );
        slurp::Globals->RenderApi->drawLine(
            vertexArrayId,
            LineVertexCount,
            shaderProgramId,
            lineWidth,
            color
        );
        slurp::Globals->RenderApi->deleteResources(
            vertexArrayId,
            render::INVALID_OBJECT_ID,
            render::INVALID_OBJECT_ID,
            shaderProgramId,
            render::INVALID_OBJECT_ID
        );
    }

    void drawRectBorder(
        const slurp::Vec2<float>& startPoint,
        const slurp::Vec2<float>& endPoint,
        float lineWidth,
        const slurp::Vec4<float>& color
    ) {
        drawLine(
            startPoint,
            {endPoint.x, startPoint.y},
            lineWidth,
            color
        );
        drawLine(
            {endPoint.x, startPoint.y},
            endPoint,
            lineWidth,
            color
        );
        drawLine(
            endPoint,
            {startPoint.x, endPoint.y},
            lineWidth,
            color
        );
        drawLine(
            {startPoint.x, endPoint.y},
            startPoint,
            lineWidth,
            color
        );
    }
}
#endif
