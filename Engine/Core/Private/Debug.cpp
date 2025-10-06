#include "Debug.h"
#if DEBUG
static constexpr uint32_t LineVertexCount = 2;

namespace debug {
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
        render::object_id shaderProgramId = slurp::GlobalRenderApi->loadShaderProgram("basic.glsl", "basic.glsl");
        render::object_id vertexArrayId = slurp::GlobalRenderApi->genVertexArrayBuffer(vertexArray, LineVertexCount);
        slurp::GlobalRenderApi->drawLine(
            vertexArrayId,
            LineVertexCount,
            shaderProgramId,
            lineWidth,
            color
        );
        // TODO: cleanup vertex array and resources
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
