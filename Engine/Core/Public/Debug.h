#pragma once

#if DEBUG
#define ASSERT(expression) if (!(expression)) *(int*)(nullptr) = 0
#else
#define ASSERT(expression)
#endif

#define VERBOSE_LOGGING 0

#define DEBUG_DRAW_COLLISION 1
#define DEBUG_RED_COLOR slurp::Vec4<float>(1.f, 0.f, 0.f, 1.f)
#define DEBUG_GREEN_COLOR slurp::Vec4<float>(0.f, 1.f, 0.f, 1.f)
#define DEBUG_BLUE_COLOR slurp::Vec4<float>(0.f, 0.f, 1.f, 1.f)
#define DEBUG_DRAW_COLOR DEBUG_GREEN_COLOR

#define DEBUG_SHOW_MOUSE_CURSOR 1

#if DEBUG
namespace debug {
    void drawLine(
        const slurp::Vec2<float>& start,
        const slurp::Vec2<float>& end,
        float lineWidth,
        const slurp::Vec4<float>& color
    );

    void drawRectBorder(
        const slurp::Vec2<float>& startPoint,
        const slurp::Vec2<float>& endPoint,
        float lineWidth,
        const slurp::Vec4<float>& color
    );
}
#endif
