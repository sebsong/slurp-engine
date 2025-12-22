#pragma once

#include "MemoryAllocator.h"
#include "Audio.h"
#include "Material.h"
#include "Mesh.h"

namespace asset {
    typedef uint32_t asset_id;

    struct FileReadResult {
        uint32_t sizeBytes;
        types::byte* contents;
    };

    struct Asset {
        uint32_t id;
        bool isLoaded;
        std::string sourceFileName;
        // TODO: ref count
    };

    struct Sprite : Asset {
        slurp::Vec2<int> dimensions;
        Mesh mesh;
        Material material;
    };

    struct Sound : Asset {
        uint32_t numSamples;
        audio::StereoAudioSampleContainer* sampleData;
    };

    struct ShaderSource : Asset {
        std::string source;
    };

    struct ShaderProgram : Asset {
        render::object_id programId;
    };
}
