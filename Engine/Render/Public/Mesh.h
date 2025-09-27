#pragma once

#include "RenderApi.h"

namespace render {
    struct Mesh {
        object_id vertexArrayId;
        uint32_t elementCount;
    };
}
