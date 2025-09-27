#pragma once

#include "RenderApi.h"

namespace render {
    struct Material {
        object_id textureId;
        object_id shaderProgramId;
    };
}
