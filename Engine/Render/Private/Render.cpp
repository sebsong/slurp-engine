#include "Render.h"

#include "Debug.h"

namespace render {
    void RenderShape::draw(const slurp::Vec2<float>& startPoint) const {
        // TODO: switch over to using the gpu render api
        // const slurp::Vec2<float> endPoint = startPoint + shape.dimensions;
        switch (shape.type) {
            case geometry::Rect: {
                // _drawRect(buffer, startPoint, endPoint, color);
                break;
            }
            default: { ASSERT(false); }
        }
    }

}
