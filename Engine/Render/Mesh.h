#pragma once

#include "RenderApi.h"

namespace asset {
    struct Mesh {
        render::object_id vertexArrayId;
        uint32_t elementCount;
    };
}
