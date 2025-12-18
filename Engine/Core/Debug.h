#pragma once

#include "Logging.h"
#include "Vector.h"

#define VERBOSE_LOGGING 0

#define DEBUG_DRAW_COLLISION 0
#define DEBUG_RED_COLOR   slurp::Vec4<float>(1, 0, 0, 1)
#define DEBUG_GREEN_COLOR slurp::Vec4<float>(0, 1, 0, 1)
#define DEBUG_BLUE_COLOR  slurp::Vec4<float>(0, 0, 1, 1)
#define DEBUG_DRAW_COLOR DEBUG_GREEN_COLOR

inline void ASSERT(bool expression) {
#if DEBUG
    if (!(expression)) {  __builtin_trap(); }
#endif
}

inline void ASSERT_LOG(bool expression, const std::string&& message) {
#if DEBUG
    if (!expression) {
        logging::error(message);
        __builtin_trap();
    }
#endif
}

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
