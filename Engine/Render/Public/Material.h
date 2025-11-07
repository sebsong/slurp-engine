#pragma once

#include "RenderApi.h"

namespace asset {
    struct Material {
        render::object_id textureId;
        render::object_id shaderProgramId;
        float alpha;
    };
}
