#pragma once

#include "RenderApi.h"

namespace asset {
    struct Material {
        render::object_id textureId;
        render::object_id shaderProgramId;
        slurp::Vec4<float> srcColor;
        slurp::Vec4<float> dstColor;
        float alpha;
    };
}
